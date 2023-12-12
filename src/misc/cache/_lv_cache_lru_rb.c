/**
* @file _lv_cache_lru_rb.c
*
*/

/*********************
 *      INCLUDES
 *********************/
#include "_lv_cache_lru_rb.h"
#include "../lv_rb.h"
#include "../lv_ll.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_lru_rb_t {
    lv_cache_t_ cache;

    lv_rb_t rb;
    lv_ll_t ll;
};
typedef struct _lv_lru_rb_t lv_lru_rb_t_;
/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * alloc_cb(void);
static bool  init_cb(lv_cache_t_ * cache);
static void  destroy_cb(lv_cache_t_ * cache, void * user_data);

static lv_cache_entry_t_ * get_or_create_cb(lv_cache_t_ * cache, const void * key, void * user_data);
static void  drop_cb(lv_cache_t_ * cache, const void * key, void * user_data);
static void  drop_all_cb(lv_cache_t_ * cache, void * user_data);

static void * alloc_new_node(lv_lru_rb_t_ * lru, void * key, void * user_data);
inline static void ** get_lru_node(lv_lru_rb_t_ * lru, lv_rb_node_t * node);
static lv_rb_compare_res_t _rb_compare_inner(const void * a, const lv_cache_entry_t_ * b);
/**********************
 *  GLOBAL VARIABLES
 **********************/
const lv_cache_class_t lv_lru_rb_class = {
    .alloc_cb = alloc_cb,
    .init_cb = init_cb,
    .destroy_cb = destroy_cb,

    .get_or_create_cb = get_or_create_cb,
    .drop_cb = drop_cb,
    .drop_all_cb = drop_all_cb
};
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void lv_lru_rb_set_max_size(lv_lru_rb_t_ * lru, size_t max_size, void * user_data)
{
    LV_UNUSED(user_data);
    lru->cache.max_size = max_size;
}

static size_t lv_lru_rb_get_max_size(lv_lru_rb_t_ * lru, void * user_data)
{
    LV_UNUSED(user_data);
    return lru->cache.max_size;
}

static size_t lv_lru_rb_get_size(lv_lru_rb_t_ * lru, void * user_data)
{
    LV_UNUSED(user_data);
    return lru->cache.size;
}

static size_t lv_lru_rb_get_free_size(lv_lru_rb_t_ * lru, void * user_data)
{
    return lv_lru_rb_get_max_size(lru, user_data) - lv_lru_rb_get_size(lru, user_data);
}

static void lv_lru_rb_set_compare_cb(lv_lru_rb_t_ * lru, lv_cache_compare_cb_t compare_cb, void * user_data)
{
    LV_UNUSED(user_data);
    lru->rb.compare = (lv_rb_compare_t)compare_cb;
}

static void lv_lru_rb_set_create_cb(lv_lru_rb_t_ * lru, lv_cache_create_cb_t create_cb, void * user_data)
{
    LV_UNUSED(user_data);
    lru->cache.create_cb = create_cb;
}

static void lv_lru_rb_set_free_cb(lv_lru_rb_t_ * lru, lv_cache_free_cb_t free_cb, void * user_data)
{
    LV_UNUSED(user_data);
    lru->cache.free_cb = free_cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void * alloc_new_node(lv_lru_rb_t_ * lru, void * key, void * user_data)
{
    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    lv_rb_node_t * node = lv_rb_insert(&lru->rb, key);
    if(node == NULL)
        goto FAILED_HANDLER3;

    lv_cache_entry_t_ * entry = (lv_cache_entry_t_ *)node->data;
    void * data = lv_malloc(lru->cache.node_size);
    LV_ASSERT_MALLOC(data);
    if(data == NULL)
        goto FAILED_HANDLER2;

    lv_cache_entry_set_data(entry, data);
    lv_memcpy(data, key, lru->cache.node_size);

    bool alloc_res = lru->cache.create_cb(data, user_data);
    if(alloc_res == false)
        goto FAILED_HANDLER2;

    void * lru_node = _lv_ll_ins_head(&lru->ll);
    if(lru_node == NULL)
        goto FAILED_HANDLER1;

    lv_memcpy(lru_node, &node, sizeof(void *));
    lv_memcpy(get_lru_node(lru, node), &lru_node, sizeof(void *));
    lv_cache_entry_set_cache(entry, (const lv_cache_t_*)lru);
    goto FAILED_HANDLER3;

FAILED_HANDLER1:
    lru->cache.free_cb(data, user_data);
FAILED_HANDLER2:
    node = NULL;
    lv_rb_drop(&lru->rb, key);
FAILED_HANDLER3:
    return node;
}

inline static void ** get_lru_node(lv_lru_rb_t_ * lru, lv_rb_node_t * node)
{
    return (void **)((char *)node->data + lru->rb.size - sizeof(void *));
}

static void * alloc_cb(void)
{
    void * res = lv_malloc(sizeof(lv_lru_rb_t_));
    LV_ASSERT_MALLOC(res);
    if(res == NULL) {
        LV_LOG_ERROR("alloc_cb: malloc failed");
        return NULL;
    }

    lv_memzero(res, sizeof(lv_lru_rb_t_));
    return res;
}

static bool  init_cb(lv_cache_t_ * cache)
{
    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru->cache.compare_cb);
    LV_ASSERT_NULL(lru->cache.create_cb);
    LV_ASSERT_NULL(lru->cache.free_cb);
    LV_ASSERT(lru->cache.node_size > 0);
    LV_ASSERT(lru->cache.max_size > 0);

    if(lru->cache.node_size == 0 || lru->cache.max_size == 0 || lru->cache.compare_cb == NULL ||
       lru->cache.create_cb == NULL || lru->cache.free_cb == NULL) {
        return false;
    }

    /*add void* to store the ll node pointer*/
    if(!lv_rb_init(&lru->rb, (lv_rb_compare_t)_rb_compare_inner,  lv_cache_entry_get_size() + sizeof(void *))) {
        return NULL;
    }
    _lv_ll_init(&lru->ll, sizeof(void *));

    lv_lru_rb_set_max_size(lru, lru->cache.max_size, NULL);
    lv_lru_rb_set_compare_cb(lru, (lv_rb_compare_t)_rb_compare_inner, NULL);
    lv_lru_rb_set_create_cb(lru, lru->cache.create_cb, NULL);
    lv_lru_rb_set_free_cb(lru, lru->cache.free_cb, NULL);

    return lru;
}

static void  destroy_cb(lv_cache_t_ * cache, void * user_data)
{
    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);

    if(lru == NULL) {
        return;
    }

    cache->clz->drop_all_cb(cache, user_data);

    lv_free(lru);
}

static lv_cache_entry_t_ * get_or_create_cb(lv_cache_t_ * cache, const void * key, void * user_data)
{
    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return NULL;
    }

    /*try the first ll node first*/
    void * head = _lv_ll_get_head(&lru->ll);
    if(head) {
        lv_rb_node_t * node = *(lv_rb_node_t **)head;
        lv_cache_entry_t_ * entry = (lv_cache_entry_t_ *)node->data;
        void * data = lv_cache_entry_get_data(entry);
        if(lru->cache.compare_cb(data, key) == 0) {
            return entry;
        }
    }

    lv_rb_node_t * node = lv_rb_find(&lru->rb, key);
    /*cache hit*/
    if(node) {
        void * lru_node = *get_lru_node(lru, node);
        head = _lv_ll_get_head(&lru->ll);
        _lv_ll_move_before(&lru->ll, lru_node, head);

        lv_cache_entry_t_ * entry = (lv_cache_entry_t_ *)node->data;
        return entry;
    }

    while(lru->cache.size >= lru->cache.max_size) {
        lv_rb_node_t * tail = *(lv_rb_node_t **)_lv_ll_get_tail(&lru->ll);
        lv_cache_entry_t_ * entry = (lv_cache_entry_t_ *)tail->data;
        void * search_key = lv_cache_entry_get_data(entry);
        cache->clz->drop_cb(cache, search_key, user_data);
    }

    /*cache miss*/
    lv_rb_node_t * new_node = alloc_new_node(lru, (void *)key, user_data);
    if(new_node == NULL) {
        return NULL;
    }

    lru->cache.size++;

    lv_cache_entry_t_ * entry = (lv_cache_entry_t_ *)new_node->data;
    return entry;
}

static void  drop_cb(lv_cache_t_ * cache, const void * key, void * user_data)
{
    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);
    LV_ASSERT_NULL(key);

    if(lru == NULL || key == NULL) {
        return;
    }

    lv_rb_node_t * node = lv_rb_find(&lru->rb, key);
    if(node == NULL) {
        return;
    }

    lv_cache_entry_t_ * entry = (lv_cache_entry_t_ *)node->data;
    void * data = lv_cache_entry_get_data(entry);

    lru->cache.free_cb(data, user_data);

    void * lru_node = *get_lru_node(lru, node);
    lv_rb_drop(&lru->rb, key);
    lv_free(data);
    _lv_ll_remove(&lru->ll, lru_node);
    lv_free(lru_node);

    lru->cache.size--;
}

static void  drop_all_cb(lv_cache_t_ * cache, void * user_data)
{
    lv_lru_rb_t_ * lru = (lv_lru_rb_t_ *)cache;

    LV_ASSERT_NULL(lru);

    if(lru == NULL) {
        return;
    }

    lv_rb_node_t ** node;
    _LV_LL_READ(&lru->ll, node) {
        /*free user handled data and do other clean up*/
        lru->cache.free_cb((*node)->data, user_data);
    }

    lv_rb_destroy(&lru->rb);
    _lv_ll_clear(&lru->ll);

    lru->cache.size = 0;
}

static lv_rb_compare_res_t _rb_compare_inner(const void * a, const lv_cache_entry_t_ * b)
{
    const lv_cache_t_ * cache = lv_cache_entry_get_cache(b);
    return cache->compare_cb(a, lv_cache_entry_get_data((lv_cache_entry_t_*)b));
}

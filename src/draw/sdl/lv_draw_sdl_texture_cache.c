/**
 * @file lv_draw_sdl_texture_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include "lv_draw_sdl_texture_cache.h"

#include "lv_draw_sdl_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    SDL_Texture * texture;
    void * userdata;
    lv_lru_free_t * userdata_free;
    lv_draw_sdl_cache_flag_t flags;
} draw_cache_value_t;

typedef struct {
    lv_sdl_cache_key_magic_t magic;
} temp_texture_key_t;

typedef struct {
    lv_coord_t width, height;
} temp_texture_userdata_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_lru_t * get_lru();

static void draw_cache_free_value(draw_cache_value_t *);

static draw_cache_value_t *draw_cache_get_entry(const void * key, size_t key_length, bool * found);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sdl_texture_cache_init(struct lv_draw_sdl_context_internals_t *internals)
{
    internals->texture_cache = lv_lru_new(LV_DRAW_SDL_LRU_SIZE, 65536,
                                                   (lv_lru_free_t *) draw_cache_free_value, NULL);
}

void lv_draw_sdl_texture_cache_deinit(struct lv_draw_sdl_context_internals_t *internals)
{
    lv_lru_free(internals->texture_cache);
}

SDL_Texture * lv_draw_sdl_texture_cache_get(const void * key, size_t key_length, bool * found)
{
    return lv_draw_sdl_texture_cache_get_with_userdata(key, key_length, found, NULL);
}

SDL_Texture * lv_draw_sdl_texture_cache_get_with_userdata(const void * key, size_t key_length, bool * found, void ** userdata)
{
    draw_cache_value_t * value = draw_cache_get_entry(key, key_length, found);
    if (!value) return NULL;
    if(userdata) {
        *userdata = value->userdata;
    }
    return value->texture;
}

void lv_draw_sdl_texture_cache_put(const void * key, size_t key_length, SDL_Texture * texture)
{
    lv_draw_sdl_texture_cache_put_advanced(key, key_length, texture, NULL, NULL, 0);
}

void lv_draw_sdl_texture_cache_put_advanced(const void * key, size_t key_length, SDL_Texture * texture, void * userdata,
                                            void userdata_free(void * ), lv_draw_sdl_cache_flag_t flags)
{
    lv_lru_t *lru = get_lru();
    draw_cache_value_t * value = SDL_malloc(sizeof(draw_cache_value_t));
    value->texture = texture;
    value->userdata = userdata;
    value->userdata_free = userdata_free;
    value->flags = flags;
    if(!texture) {
        lv_lru_set(lru, key, key_length, value, 1);
        return;
    }
    if(flags & LV_DRAW_SDL_CACHE_FLAG_MANAGED) {
        /* Managed texture doesn't count into cache size */
        LV_LOG_INFO("cache texture %p, %d*%d@%dbpp", texture, width, height, SDL_BITSPERPIXEL(format));
        lv_lru_set(lru, key, key_length, value, 1);
        return;
    }
    Uint32 format;
    int access, width, height;
    if(SDL_QueryTexture(texture, &format, &access, &width, &height) != 0) {
        return;
    }
    LV_LOG_INFO("cache texture %p, %d*%d@%dbpp", texture, width, height, SDL_BITSPERPIXEL(format));
    lv_lru_set(lru, key, key_length, value, width * height * SDL_BITSPERPIXEL(format) / 8);
}

lv_draw_sdl_cache_key_head_img_t * lv_draw_sdl_texture_img_key_create(const void * src, int32_t frame_id, size_t * size)
{
    lv_draw_sdl_cache_key_head_img_t header;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&header, 0, sizeof(header));
    header.magic = LV_GPU_CACHE_KEY_MAGIC_IMG;
    header.type = lv_img_src_get_type(src);
    header.frame_id = frame_id;
    void * key;
    size_t key_size;
    if(header.type == LV_IMG_SRC_FILE || header.type == LV_IMG_SRC_SYMBOL) {
        size_t srclen = SDL_strlen(src);
        key_size = sizeof(header) + srclen;
        key = SDL_malloc(key_size);
        SDL_memcpy(key, &header, sizeof(header));
        /*Copy string content as key value*/
        SDL_memcpy(key + sizeof(header), src, srclen);
    }
    else {
        key_size = sizeof(header) + sizeof(void *);
        key = SDL_malloc(key_size);
        SDL_memcpy(key, &header, sizeof(header));
        /*Copy address number as key value*/
        SDL_memcpy(key + sizeof(header), &src, sizeof(void *));
    }
    *size = key_size;
    return (lv_draw_sdl_cache_key_head_img_t *) key;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_lru_t * get_lru()
{
    return lv_draw_sdl_get_context()->internals->texture_cache;
}

static void draw_cache_free_value(draw_cache_value_t * value)
{
    if(value->texture && !(value->flags & LV_DRAW_SDL_CACHE_FLAG_MANAGED)) {
        LV_LOG_INFO("destroy texture %p", value->texture);
        SDL_DestroyTexture(value->texture);
    }
    if(value->userdata_free) {
        value->userdata_free(value->userdata);
    }
    SDL_free(value);
}

static draw_cache_value_t *draw_cache_get_entry(const void * key, size_t key_length, bool * found)
{
    lv_lru_t *lru = get_lru();
    draw_cache_value_t * value = NULL;
    lv_lru_get(lru, key, key_length, (void **) &value);
    if(!value) {
        if(found) {
            *found = false;
        }
        return NULL;
    }
    if(found) {
        *found = true;
    }
    return value;
}

#endif /*LV_USE_DRAW_SDL*/


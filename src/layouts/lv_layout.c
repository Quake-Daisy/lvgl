/**
 * @file lv_layout.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_layout.h"
#include "../misc/lv_gc.h"
#include "../core/lv_global.h"
#include "../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#define layout_cnt lv_global_default()->layout_count
#define layout_list_def lv_global_default()->layout_list

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_layout_init(void)
{
    /*Malloc a list for the built in layouts*/
    layout_list_def = lv_malloc(layout_cnt * sizeof(lv_layout_dsc_t));

#if LV_USE_FLEX
    lv_flex_init();
#endif

#if LV_USE_GRID
    lv_grid_init();
#endif
}

uint32_t lv_layout_register(lv_layout_update_cb_t cb, void * user_data)
{
    layout_cnt++;
    layout_list_def = lv_realloc(layout_list_def, layout_cnt * sizeof(lv_layout_dsc_t));
    LV_ASSERT_MALLOC(layout_list_def);

    layout_list_def[layout_cnt - 1].cb = cb;
    layout_list_def[layout_cnt - 1].user_data = user_data;
    return layout_cnt;  /*No -1 to skip 0th index*/
}

void _lv_layout_apply(lv_obj_t * obj)
{
    lv_layout_t layout_id = lv_obj_get_style_layout(obj, LV_PART_MAIN);
    if(layout_id > 0 && layout_id <= layout_cnt) {
        void  * user_data = layout_list_def[layout_id].user_data;
        layout_list_def[layout_id].cb(obj, user_data);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

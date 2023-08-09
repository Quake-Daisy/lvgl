/**
 * @file lv_gc.h
 *
 */

#ifndef LV_GC_H
#define LV_GC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include <stdint.h>
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"
#include "lv_ll.h"
#include "lv_timer.h"
#include "lv_types.h"
#include "../draw/lv_img_cache.h"
#include "../draw/sw/lv_draw_sw_mask.h"
#include "../layouts/lv_layout.h"
#include "../disp/lv_disp.h"

/*********************
 *      DEFINES
 *********************/
#if LV_IMG_CACHE_DEF_SIZE
#    define LV_IMG_CACHE_DEF            1
#else
#    define LV_IMG_CACHE_DEF            0
#endif

#define LV_DISPATCH(f, t, n)            f(t, n)
#define LV_DISPATCH_COND(f, t, n, m, v) LV_CONCAT3(LV_DISPATCH, m, v)(f, t, n)

#define LV_DISPATCH00(f, t, n)          LV_DISPATCH(f, t, n)
#define LV_DISPATCH01(f, t, n)
#define LV_DISPATCH10(f, t, n)
#define LV_DISPATCH11(f, t, n)          LV_DISPATCH(f, t, n)

#define LV_ITERATE_ROOTS(f)                                                                            \
    LV_DISPATCH_COND(f, _lv_img_cache_entry_t*, _lv_img_cache_array, LV_IMG_CACHE_DEF, 1)              \
    LV_DISPATCH_COND(f, _lv_img_cache_entry_t, _lv_img_cache_single, LV_IMG_CACHE_DEF, 0)              \
    LV_DISPATCH(f, lv_timer_t*, _lv_timer_act)                                                         \
    LV_DISPATCH_COND(f, _lv_draw_sw_mask_radius_circle_dsc_arr_t , _lv_circle_cache, LV_DRAW_SW_COMPLEX, 1)  \
    LV_DISPATCH(f, void * , _lv_theme_default_data)                                                  \
    LV_DISPATCH(f, void * , _lv_theme_mono_data)                                                    \
    LV_DISPATCH(f, void * , _lv_theme_basic_data)                                                    \
    LV_DISPATCH(f, uint8_t * , _lv_style_custom_prop_flag_lookup_table)                                \
    LV_DISPATCH(f, lv_draw_unit_t *, _lv_draw_unit_head)                                               \
    LV_DISPATCH(f, lv_ll_t, _subs_ll)

#define LV_DEFINE_ROOT(root_type, root_name) root_type root_name;
#define LV_ROOTS LV_ITERATE_ROOTS(LV_DEFINE_ROOT)

#if LV_ENABLE_GC == 1
#if LV_USE_BUILTIN_MALLOC
#error "GC requires CUSTOM_MEM"
#endif /*LV_USE_BUILTIN_MALLOC*/
#include LV_GC_INCLUDE
#else  /*LV_ENABLE_GC*/
#define LV_GC_ROOT(x) x
#define LV_EXTERN_ROOT(root_type, root_name) extern root_type root_name;
LV_ITERATE_ROOTS(LV_EXTERN_ROOT)
#endif /*LV_ENABLE_GC*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _lv_gc_clear_roots(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GC_H*/

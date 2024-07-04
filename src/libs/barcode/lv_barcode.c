/**
 * @file lv_barcode.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_barcode.h"
#include "../../lvgl.h"

#if LV_USE_BARCODE

#include "code128.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_barcode_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_barcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_barcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static bool lv_barcode_change_buf_size(lv_obj_t * obj, int32_t w, int32_t h);
static void lv_barcode_clear(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_barcode_class = {
    .constructor_cb = lv_barcode_constructor,
    .destructor_cb = lv_barcode_destructor,
    .width_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_barcode_t),
    .base_class = &lv_canvas_class,
    .name = "barcode",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_barcode_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_barcode_set_dark_color(lv_obj_t * obj, lv_color_t color)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->dark_color = color;
}

void lv_barcode_set_light_color(lv_obj_t * obj, lv_color_t color)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->light_color = color;
}

void lv_barcode_set_scale(lv_obj_t * obj, uint16_t scale)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(scale == 0) {
        scale = 1;
    }

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->scale = scale;
}

void lv_barcode_set_direction(lv_obj_t * obj, lv_dir_t direction)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->direction = direction;
}

void lv_barcode_set_tiled(lv_obj_t * obj, bool tiled)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->tiled = tiled;
    lv_image_set_inner_align(obj, tiled ? LV_IMAGE_ALIGN_TILE : LV_IMAGE_ALIGN_DEFAULT);
}

lv_result_t lv_barcode_update(lv_obj_t * obj, const char * data)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(data);

    lv_result_t res = LV_RESULT_INVALID;
    lv_barcode_t * barcode = (lv_barcode_t *)obj;

    if(data == NULL || lv_strlen(data) == 0) {
        LV_LOG_WARN("data is empty");
        lv_barcode_clear(obj);
        return LV_RESULT_INVALID;
    }

    size_t len = code128_estimate_len(data);
    LV_LOG_INFO("data: %s, len = %zu", data, len);

    char * out_buf = lv_malloc(len);
    LV_ASSERT_MALLOC(out_buf);
    if(!out_buf) {
        LV_LOG_ERROR("malloc failed for out_buf");
        lv_barcode_clear(obj);
        return LV_RESULT_INVALID;
    }

    int32_t barcode_w = (int32_t) code128_encode_gs1(data, out_buf, len);
    LV_LOG_INFO("barcode width = %d", (int)barcode_w);

    LV_ASSERT(barcode->scale > 0);
    uint16_t scale = barcode->scale;

    int32_t buf_w;
    int32_t buf_h;

    if(barcode->tiled) {
        buf_w = (barcode->direction == LV_DIR_HOR) ? barcode_w * scale : 1;
        buf_h = (barcode->direction == LV_DIR_VER) ? barcode_w * scale : 1;
    }
    else {
        lv_obj_update_layout(obj);
        buf_w = (barcode->direction == LV_DIR_HOR) ? barcode_w * scale : lv_obj_get_width(obj);
        buf_h = (barcode->direction == LV_DIR_VER) ? barcode_w * scale : lv_obj_get_height(obj);
    }

    if(!lv_barcode_change_buf_size(obj, buf_w, buf_h)) {
        lv_barcode_clear(obj);
        goto failed;
    }

    /* Temporarily disable invalidation to improve the efficiency of lv_canvas_set_px */
    lv_display_enable_invalidation(lv_obj_get_display(obj), false);

    lv_canvas_set_palette(obj, 0, lv_color_to_32(barcode->dark_color, 0xff));
    lv_canvas_set_palette(obj, 1, lv_color_to_32(barcode->light_color, 0xff));

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    uint32_t stride = draw_buf->header.stride;
    uint8_t * dest = lv_draw_buf_goto_xy(draw_buf, 0, 0);

    for(int32_t x = 0; x < barcode_w; x++) {
        lv_color_t color = lv_color_hex(out_buf[x] ? 0 : 1);

        for(uint16_t i = 0; i < scale; i++) {
            if(barcode->direction == LV_DIR_HOR) {
                lv_canvas_set_px(obj, x * scale + i, 0, color, LV_OPA_COVER);
            }
            else {
                if(barcode->tiled) {
                    lv_canvas_set_px(obj, 0, x * scale + i, color, LV_OPA_COVER);
                }
                else {
                    lv_memset(dest, lv_color_to_int(color) ? 0xFF : 0x00, stride);
                    dest += stride;
                }
            }
        }
    }

    /* Copy pixels by row */
    if(!barcode->tiled && barcode->direction == LV_DIR_HOR && buf_h > 1) {
        /* Skip the first row */
        int32_t h = buf_h - 1;
        uint8_t * src = lv_draw_buf_goto_xy(draw_buf, 0, 0);
        dest = src + stride;
        while(h--) {
            lv_memcpy(dest, src, stride);
            dest += stride;
        }
    }

    lv_display_enable_invalidation(lv_obj_get_display(obj), true);
    lv_obj_invalidate(obj);

    res = LV_RESULT_OK;

failed:
    lv_free(out_buf);
    return res;
}

lv_color_t lv_barcode_get_dark_color(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->dark_color;
}

lv_color_t lv_barcode_get_light_color(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->light_color;
}

uint16_t lv_barcode_get_scale(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    return barcode->scale;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_barcode_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_barcode_t * barcode = (lv_barcode_t *)obj;
    barcode->dark_color = lv_color_black();
    barcode->light_color = lv_color_white();
    barcode->scale = 1;
    barcode->direction = LV_DIR_HOR;
}

static void lv_barcode_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(draw_buf == NULL) return;
    lv_image_cache_drop(draw_buf);

    /*@fixme destroy buffer in cache free_cb.*/
    lv_draw_buf_destroy(draw_buf);
}

static bool lv_barcode_change_buf_size(lv_obj_t * obj, int32_t w, int32_t h)
{
    LV_ASSERT_NULL(obj);
    if(w <= 0 || h <= 0) {
        LV_LOG_WARN("invalid size: %" LV_PRId32 " x %" LV_PRId32, w, h);
        return false;
    }

    lv_draw_buf_t * old_buf = lv_canvas_get_draw_buf(obj);
    lv_draw_buf_t * new_buf = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_I1, LV_STRIDE_AUTO);
    if(new_buf == NULL) {
        LV_LOG_ERROR("malloc failed for canvas buffer");
        return false;
    }

    lv_canvas_set_draw_buf(obj, new_buf);
    LV_LOG_INFO("set canvas buffer: %p, width = %d", (void *)new_buf, (int)w);

    if(old_buf != NULL) lv_draw_buf_destroy(old_buf);
    return true;
}

static void lv_barcode_clear(lv_obj_t * obj)
{
    lv_draw_buf_t * draw_buf = lv_canvas_get_draw_buf(obj);
    if(!draw_buf) {
        return;
    }

    lv_draw_buf_clear(draw_buf, NULL);
    lv_image_cache_drop(draw_buf);
    lv_obj_invalidate(obj);
}

#endif /*LV_USE_BARCODE*/

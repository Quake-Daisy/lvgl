/**
 * @file lv_snapshot.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>

#include "lv_snapshot.h"
#include "../core/lv_disp.h"
#include "../core/lv_refr.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void set_set_px_cb(lv_disp_drv_t * disp_drv, lv_img_cf_t cf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/** Take snapshot for object with its children.
 *
 * @note The function will change the order obj in its parent.
 *
 * @param obj    The object to generate snapshot.
 * @param cf     color format for generated image.
 *
 * @return a pointer to a image descriptor, or NULL if failed.
 */
lv_img_dsc_t * lv_snapshot_take(lv_obj_t * obj, lv_img_cf_t cf)
{
    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
        case LV_IMG_CF_ALPHA_1BIT:
        case LV_IMG_CF_ALPHA_2BIT:
        case LV_IMG_CF_ALPHA_4BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            break;
        default:
            return NULL;
    }

    /*Backup obj original info.*/
    lv_disp_t * disp_old = lv_disp_get_default();
    lv_obj_t * parent_old = lv_obj_get_parent(obj);
    lv_coord_t w, h, x, y;

    lv_obj_update_layout(obj);

    /*Width and height determine snapshot image size.*/
    w = lv_obj_get_width(obj);
    h = lv_obj_get_height(obj);

    /*Position will be set back to obj once done. */
    x = lv_obj_get_style_x(obj, LV_PART_MAIN);
    y = lv_obj_get_style_y(obj, LV_PART_MAIN);
    lv_align_t align = lv_obj_get_style_align(obj, LV_PART_MAIN);

    uint8_t px_size = lv_img_cf_get_px_size(cf);
    uint32_t buff_size = w * h * ((px_size + 7) >> 3);
    void * buff = lv_mem_alloc(buff_size);
    if(buff == NULL) {
        return NULL;
    }

    lv_img_dsc_t * dsc = lv_mem_alloc(sizeof(lv_img_dsc_t));
    if(dsc == NULL) {
        lv_mem_free(buff);
        return NULL;
    }
    lv_memset(buff, 0x00, buff_size);
    lv_memset_00(dsc, sizeof(lv_img_dsc_t));

    /*We are safe to use stack for below variables since disp will be
     * unregistered when function returns. */
    lv_disp_t * disp;
    lv_disp_drv_t driver;
    lv_disp_draw_buf_t draw_buf;

    lv_disp_draw_buf_init(&draw_buf, buff, NULL, w * h);
    draw_buf.area.x1 = 0;
    draw_buf.area.x2 = w - 1;
    draw_buf.area.y1 = 0;
    draw_buf.area.y2 = h - 1;

    lv_disp_drv_init(&driver);
    driver.draw_buf = &draw_buf;
    driver.hor_res = w;
    driver.ver_res = h;
    set_set_px_cb(&driver, cf);

    disp = lv_disp_drv_register(&driver);
    if(disp == NULL) {
        lv_mem_free(buff);
        lv_mem_free(dsc);
        return NULL;
    }

    /*Make background transparent */
    lv_disp_set_bg_opa(disp, LV_OPA_TRANSP);

    /*Move obj to newly created disp and refresh it. */
    lv_disp_set_default(disp);

    lv_obj_t * screen = lv_scr_act();
    lv_obj_allocate_spec_attr(screen);
    screen->spec_attr->child_cnt = 1;
    screen->spec_attr->children = &obj;

    obj->parent = screen;

    /*Display buffer targets to position 0, 0 */
    lv_obj_align(obj, LV_ALIGN_DEFAULT, 0, 0);
    lv_obj_invalidate(obj);

    /*Don't call lv_refr_now to avoid animation disruption */
    _lv_disp_refr_timer(disp->refr_timer);

    /*Restore obj original parameters. */
    lv_disp_set_default(disp_old);

    obj->parent = parent_old;
    screen->spec_attr->child_cnt = 0;
    screen->spec_attr->children = NULL;

    /*Unregister temp display driver. */
    lv_disp_remove(disp);

    lv_obj_align(obj, align, x, y);

    lv_obj_invalidate(obj);

    dsc->data = buff;
    dsc->header.w = w;
    dsc->header.h = h;
    dsc->header.cf = cf;
    return dsc;
}

/** Free the snapshot image returned by @ref lv_snapshot_take
 *
 * It will firstly free the data image takes, then the image descriptor.
 *
 * @param dsc    The image descriptor generated by lv_snapshot_take.
 *
 */
void lv_snapshot_free(lv_img_dsc_t * dsc)
{
    if(!dsc)
        return;

    if(dsc->data)
        lv_mem_free((void *)dsc->data);

    lv_mem_free(dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void set_px_true_color_alpha(lv_disp_drv_t * disp_drv, uint8_t * buf,
                                    lv_coord_t buf_w, lv_coord_t x,
                                    lv_coord_t y, lv_color_t color,
                                    lv_opa_t opa);

static void set_px_cb_alpha1(lv_disp_drv_t * disp_drv, uint8_t * buf,
                             lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa);

static void set_px_cb_alpha2(lv_disp_drv_t * disp_drv, uint8_t * buf,
                             lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa);

static void set_px_cb_alpha4(lv_disp_drv_t * disp_drv, uint8_t * buf,
                             lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa);

static void set_px_cb_alpha8(lv_disp_drv_t * disp_drv, uint8_t * buf,
                             lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa);

static void set_px_alpha_generic(lv_img_dsc_t * d, lv_coord_t x, lv_coord_t y,
                                 lv_color_t color, lv_opa_t opa);

static void set_px_cb_alpha1(lv_disp_drv_t * disp_drv, uint8_t * buf,
                             lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa)
{
    (void)disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN)
        return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_ALPHA_1BIT;

    set_px_alpha_generic(&d, x, y, color, opa);
}

static void set_px_cb_alpha2(lv_disp_drv_t * disp_drv, uint8_t * buf,
                             lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa)
{
    (void)disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN)
        return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_ALPHA_2BIT;

    set_px_alpha_generic(&d, x, y, color, opa);
}

static void set_px_cb_alpha4(lv_disp_drv_t * disp_drv, uint8_t * buf,
                             lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa)
{
    (void)disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN)
        return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_ALPHA_4BIT;

    set_px_alpha_generic(&d, x, y, color, opa);
}

static void set_px_cb_alpha8(lv_disp_drv_t * disp_drv, uint8_t * buf,
                             lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                             lv_color_t color, lv_opa_t opa)
{
    (void)disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN)
        return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_ALPHA_8BIT;

    set_px_alpha_generic(&d, x, y, color, opa);
}

static void set_px_alpha_generic(lv_img_dsc_t * d, lv_coord_t x, lv_coord_t y,
                                 lv_color_t color, lv_opa_t opa)
{
    d->header.always_zero = 0;
    d->header.h = 1; /*Doesn't matter*/

    uint8_t br = lv_color_brightness(color);
    if(opa < LV_OPA_MAX) {
        uint8_t bg = lv_img_buf_get_px_alpha(d, x, y);
        br = (uint16_t)((uint16_t)br * opa + (bg * (255 - opa))) >> 8;
    }

    lv_img_buf_set_px_alpha(d, x, y, br);
}

static void set_px_true_color_alpha(lv_disp_drv_t * disp_drv, uint8_t * buf,
                                    lv_coord_t buf_w, lv_coord_t x,
                                    lv_coord_t y, lv_color_t color,
                                    lv_opa_t opa)
{
    (void)disp_drv; /*Unused*/

    if(opa <= LV_OPA_MIN)
        return;
    lv_img_dsc_t d;
    d.data = buf;
    d.header.always_zero = 0;
    d.header.h = 1; /*Doesn't matter*/
    ;
    d.header.w = buf_w;
    d.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;

    lv_color_t bg_color = lv_img_buf_get_px_color(&d, x, y, lv_color_black());
    lv_opa_t bg_opa = lv_img_buf_get_px_alpha(&d, x, y);

    lv_opa_t res_opa;
    lv_color_t res_color;

    lv_color_mix_with_alpha(bg_color, bg_opa, color, opa, &res_color, &res_opa);

    lv_img_buf_set_px_alpha(&d, x, y, res_opa);
    lv_img_buf_set_px_color(&d, x, y, res_color);
}

static void set_set_px_cb(lv_disp_drv_t * disp_drv, lv_img_cf_t cf)
{
    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
            disp_drv->set_px_cb = set_px_true_color_alpha;
            break;
        case LV_IMG_CF_ALPHA_1BIT:
            disp_drv->set_px_cb = set_px_cb_alpha1;
            break;
        case LV_IMG_CF_ALPHA_2BIT:
            disp_drv->set_px_cb = set_px_cb_alpha2;
            break;
        case LV_IMG_CF_ALPHA_4BIT:
            disp_drv->set_px_cb = set_px_cb_alpha4;
            break;
        case LV_IMG_CF_ALPHA_8BIT:
            disp_drv->set_px_cb = set_px_cb_alpha8;
            break;
        default:
            disp_drv->set_px_cb = NULL;
    }
}

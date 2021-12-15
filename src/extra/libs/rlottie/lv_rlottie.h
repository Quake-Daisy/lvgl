/**
 * @file lv_rlottie.h
 *
 */

#ifndef LV_RLOTTIE_H
#define LV_RLOTTIE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_RLOTTIE

#include <rlottie_capi.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_RLOTTIE_CTRL_FORWARD  = 0,
    LV_RLOTTIE_CTRL_BACKWARD = 1,
    LV_RLOTTIE_CTRL_PAUSE    = 2,
    LV_RLOTTIE_CTRL_PLAY     = 0, /* Yes, play = 0 is the default mode */
    LV_RLOTTIE_CTRL_LOOP     = 8,
    LV_RLOTTIE_CTRL_STOPAT   = 4,
} lv_rlottie_ctrl_t;

typedef struct {
    lv_img_t img_ext;
    lv_img_decoder_dsc_t * dec;
    lv_timer_t * task;
    size_t total_frames;
    size_t current_frame;
    size_t framerate;
    lv_rlottie_ctrl_t play_ctrl;
    size_t dest_frame;
} lv_rlottie_t;

extern const lv_obj_class_t lv_rlottie_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_rlottie_create_from_file(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * path);

lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, lv_coord_t width, lv_coord_t height,
                                      const char * rlottie_desc);

void lv_rlottie_set_play_mode(lv_obj_t * rlottie, const lv_rlottie_ctrl_t ctrl);
void lv_rlottie_set_current_frame(lv_obj_t * rlottie, const size_t goto_frame);
void lv_rlottie_stopat_frame(lv_obj_t * rlottie, const size_t goto_frame, const int forward);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLOTTIE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_RLOTTIE_H*/

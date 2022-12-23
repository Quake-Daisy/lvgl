/**
 * @file lv_animimg.h
 *
 */

#ifndef LV_ANIM_IMG_H
#define LV_ANIM_IMG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../img/lv_img.h"

#if LV_USE_ANIMIMG != 0

/*Testing of dependencies*/
#if LV_USE_IMG == 0
#error "lv_animimg: lv_img is required. Enable it in lv_conf.h (LV_USE_IMG 1)"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

extern const lv_obj_class_t lv_animimg_class;

/*Data of image*/
typedef struct {
    lv_img_t img;
    lv_anim_t anim;
} lv_animimg_t;


/*Image parts*/
enum {
    LV_ANIM_IMG_PART_MAIN,
};
typedef uint8_t lv_animimg_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an animation image objects
 * @param parent pointer to an object, it will be the parent of the new button
 * @return pointer to the created animation image object
 */
lv_obj_t * lv_animimg_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the image animation images source.
 * @param img  pointer to an animation image object
 * @param srcs pointer to a series images, captured (use lv_img_src_from_arr likely)
 */
void lv_animimg_set_src(lv_obj_t * img,  lv_img_src_t * srcs);

/**
 * Startup the image animation.
 * @param obj pointer to an animation image object
 */
void lv_animimg_start(lv_obj_t * obj);

/**
 * Set the image animation duration time. unit:ms
 * @param img pointer to an animation image object
 */
void lv_animimg_set_duration(lv_obj_t * img, uint32_t duration);

/**
 * Set the image animation repeatly play times.
 * @param img pointer to an animation image object
 * @param count the number of times to repeat the animation
 */
void lv_animimg_set_repeat_count(lv_obj_t * img, uint16_t count);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the image animation images source.
 * @param img pointer to an animation image object
 * @return a pointer that will point to an image source array that shouldn't be cleared.
 */
lv_img_src_t * lv_animimg_get_src(lv_obj_t * img);

/**
 * Get the image animation images source.
 * @param img pointer to an animation image object
 * @return the number of source images
 */
uint8_t lv_animimg_get_src_count(lv_obj_t * img);

/**
 * Get the image animation duration time. unit:ms
 * @param img pointer to an animation image object
 * @return the animation duration time
 */
uint32_t lv_animimg_get_duration(lv_obj_t * img);

/**
 * Get the image animation repeat play times.
 * @param img pointer to an animation image object
 * @return the repeat count
 */
uint16_t lv_animimg_get_repeat_count(lv_obj_t * img);

#endif /*LV_USE_ANIMIMG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_ANIM_IMG_H*/

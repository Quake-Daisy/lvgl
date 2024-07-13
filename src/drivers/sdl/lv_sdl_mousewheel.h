/**
 * @file lv_sdl_mousewheel.h
 *
 */

#ifndef LV_SDL_MOUSEWHEEL_H
#define LV_SDL_MOUSEWHEEL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_window.h"
#if LV_USE_SDL && LV_SDL_MOUSEWHEEL_MODE == LV_SDL_MOUSEWHEEL_MODE_ENCODER

#include LV_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_indev_t * lv_sdl_mousewheel_create(void);
void _lv_sdl_mousewheel_handler(SDL_Event * event);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DEV_SDL_MOUSEWHEEL_H */

/**
 * @file Lv_sdl_mouse.h
 *
 */

#ifndef LV_SDL_MOUSE_H
#define LV_SDL_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_window.h"
#if LV_USE_SDL

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

lv_indev_t * lv_sdl_mouse_create(void);
void _lv_sdl_mouse_handler(SDL_Event * event);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_MOUSE_H */

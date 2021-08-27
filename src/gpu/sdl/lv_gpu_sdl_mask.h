//
// Created by Mariotaku on 2021/08/23.
//

#ifndef LVGL_SDL_EXAMPLE_LV_GPU_SDL_MASK_H
#define LVGL_SDL_EXAMPLE_LV_GPU_SDL_MASK_H

#include <SDL.h>

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"


lv_opa_t *lv_draw_mask_dump(const lv_area_t *coords, const int16_t *ids, int16_t ids_count);

SDL_Surface *lv_sdl_create_mask_surface(lv_opa_t *pixels, lv_coord_t width, lv_coord_t height, lv_coord_t stride);

SDL_Texture *lv_sdl_create_mask_texture(SDL_Renderer *renderer, lv_opa_t *pixels, lv_coord_t width,
                                         lv_coord_t height, lv_coord_t stride);

SDL_Surface *lv_sdl_apply_mask_surface(const lv_area_t *coords, const int16_t *ids, int16_t ids_count);

SDL_Texture *lv_sdl_gen_mask_texture(SDL_Renderer *renderer, const lv_area_t *coords, const int16_t *ids, int16_t ids_count);

#endif //LVGL_SDL_EXAMPLE_LV_GPU_SDL_MASK_H

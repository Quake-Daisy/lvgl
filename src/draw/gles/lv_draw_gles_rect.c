/**
 * @file lv_draw_gles_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

#include "../lv_draw.h"
#include "lv_draw_gles_utils.h"
#include "lv_draw_gles.h"

#include LV_GPU_SDL_GLES_GLAD_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
void opengl_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/
#define SKIP_BORDER(dsc) ((dsc)->border_opa <= LV_OPA_MIN || (dsc)->border_width == 0 || (dsc)->border_side == LV_BORDER_SIDE_NONE || (dsc)->border_post)
#define SKIP_SHADOW(dsc) ((dsc)->shadow_width == 0 || (dsc)->shadow_opa <= LV_OPA_MIN || ((dsc)->shadow_width == 1 && (dsc)->shadow_spread <= 0 && (dsc)->shadow_ofs_x == 0 && (dsc)->shadow_ofs_y == 0))
#define SKIP_IMAGE(dsc) ((dsc)->bg_img_src == NULL || (dsc)->bg_img_opa <= LV_OPA_MIN)
#define SKIP_OUTLINE(dsc) ((dsc)->outline_opa <= LV_OPA_MIN || (dsc)->outline_width == 0)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_sw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_gles_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    /* Do software drawing */
    //lv_draw_sw_rect(draw_ctx, dsc, coords);

    //lv_draw_gles_utils_upload_texture(draw_ctx);
    /* Do opengl drawing */
    opengl_draw_rect(draw_ctx, dsc, coords);
    lv_draw_gles_utils_download_texture(draw_ctx);
    // return;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void opengl_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{

    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;

    if(dsc->bg_opa <= LV_OPA_MIN) return;

    lv_area_t bg_coords;
    lv_area_copy(&bg_coords, coords);

    /*If the border fully covers make the bg area 1px smaller to avoid artifacts on the corners*/
    if(dsc->border_width > 1 && dsc->border_opa >= LV_OPA_MAX && dsc->radius != 0) {
        bg_coords.x1 += (dsc->border_side & LV_BORDER_SIDE_LEFT) ? 1 : 0;
        bg_coords.y1 += (dsc->border_side & LV_BORDER_SIDE_TOP) ? 1 : 0;
        bg_coords.x2 -= (dsc->border_side & LV_BORDER_SIDE_RIGHT) ? 1 : 0;
        bg_coords.y2 -= (dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? 1 : 0;
    }

    lv_area_t clipped_coords;
    if(!_lv_area_intersect(&clipped_coords, &bg_coords, draw_ctx->clip_area)) return;

    lv_grad_dir_t grad_dir = dsc->bg_grad.dir;
    lv_color_t bg_color    = grad_dir == LV_GRAD_DIR_NONE ? dsc->bg_color : dsc->bg_grad.stops[0].color;
    if(bg_color.full == dsc->bg_grad.stops[1].color.full) grad_dir = LV_GRAD_DIR_NONE;

    bool mask_any = lv_draw_mask_is_any(&bg_coords);
    lv_draw_sw_blend_dsc_t blend_dsc = {0};
    blend_dsc.blend_mode = dsc->blend_mode;
    blend_dsc.color = bg_color;

    lv_opa_t opa = dsc->bg_opa >= LV_OPA_MAX ? LV_OPA_COVER : dsc->bg_opa;

    /*Get the real radius. Can't be larger than the half of the shortest side */
    lv_coord_t coords_bg_w = lv_area_get_width(&bg_coords);
    lv_coord_t coords_bg_h = lv_area_get_height(&bg_coords);
    int32_t short_side = LV_MIN(coords_bg_w, coords_bg_h);
    int32_t rout = LV_MIN(dsc->radius, short_side >> 1);


    /*Add a radius mask if there is radius*/
    int32_t clipped_w = lv_area_get_width(&clipped_coords);
    int16_t mask_rout_id = LV_MASK_ID_INV;
    lv_opa_t * mask_buf = NULL;
    lv_draw_mask_radius_param_t mask_rout_param;
    if(rout > 0 || mask_any) {
        LV_LOG_USER("rout : %d coords: x1 :%d x2: %d y1 : %d y2 : %d", dsc->radius, bg_coords.x1, bg_coords.x2, bg_coords.y1, bg_coords.y2);
      //  mask_buf = lv_mem_buf_get(clipped_w);
        //lv_draw_mask_radius_init(&mask_rout_param, &bg_coords, rout, false);
       // mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
    }

    vec2 resolution;
    resolution[0] = (float)coords->x2 - (float)coords->x1;
    resolution[1] = (float)coords->y2 - (float)coords->y1;

    vec4 color;
    color[0] = (float)bg_color.ch.red/255.0f;
    color[1] = (float)bg_color.ch.green/255.0f;
    color[2] = (float)bg_color.ch.blue/255.0f;
    color[3] = (float)bg_color.ch.alpha/255.0f;

    vec4 shader_coords;
    shader_coords[0] = (float)coords->x1;
    shader_coords[1] = (float)coords->x2;
    shader_coords[2] = (float)coords->y1;
    shader_coords[3] = (float)coords->y2;

    static GLfloat vertices[] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3) {coords->x1, coords->y2});
    glm_scale(model, (vec3) {coords->x2 - coords->x1, coords->y1 - coords->y2});
    LV_LOG_USER("coords: x1 :%d x2: %d y1 : %d y2 : %d", coords->x1,
                                                         coords->x2,
                coords->y1,
                coords->y2);



                glBindFramebuffer(GL_FRAMEBUFFER, internals->framebuffer);
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(internals->rect_shader);

    glUniform4f(internals->rect_shader_color_location, color[0], color[1], color[2], color[3]);
    glUniform4f(internals->rect_shader_coords_location, shader_coords[0], shader_coords[1], shader_coords[2], shader_coords[3]);
    glUniform2f(internals->rect_shader_resolution_location, resolution[0], resolution[1]);

    glUniform1f(internals->rect_shader_radius_location, (float)rout);
    glUniformMatrix4fv(internals->rect_shader_model_location, 1, GL_FALSE, &model[0][0]);
    glVertexAttribPointer(internals->rect_shader_pos_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), vertices);
    glEnableVertexAttribArray(internals->rect_shader_pos_location);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

#endif /*LV_USE_GPU_SDL_GLES*/

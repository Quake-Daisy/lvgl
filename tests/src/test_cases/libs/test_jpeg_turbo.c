#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "lv_test_helpers.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

static void create_images(void)
{
    lv_obj_clean(lv_scr_act());

    lv_obj_t * img;

    img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, "A:src/test_assets/test_img_lvgl_logo.jpg");
    lv_obj_center(img);
}

void test_jpg_2(void)
{
    create_images();

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_2.png");


    uint32_t mem_before = lv_test_get_free_mem();
    for(uint32_t i = 0; i < 20; i++) {
        create_images();

        lv_obj_invalidate(lv_scr_act());
        lv_refr_now(NULL);
    }

    TEST_ASSERT_EQUAL_SCREENSHOT("libs/jpg_2.png");

    TEST_ASSERT_EQUAL(mem_before, lv_test_get_free_mem());

}

#endif

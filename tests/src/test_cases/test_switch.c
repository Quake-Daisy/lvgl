#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#include "lv_test_indev.h"

uint8_t value_changed = 0;
lv_obj_t *scr = NULL;
lv_obj_t *sw = NULL;

void setUp(void)
{
    /* Function run before every test */
    scr = lv_scr_act();
    sw = lv_switch_create(scr);
}

void tearDown(void)
{
    /* Function run after every test */
    value_changed = 0;
}

static void event_handler(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (LV_EVENT_VALUE_CHANGED == event) {
        value_changed++;
    }

}

void test_switch_should_have_default_state_after_being_created(void)
{
    lv_state_t state = lv_obj_get_state(sw);
    TEST_ASSERT_EQUAL(state, LV_STATE_DEFAULT);
}

/* See #2330 for context */
void test_switch_should_trigger_value_changed_event_only_once(void)
{
    lv_obj_add_event_cb(sw, event_handler, LV_EVENT_ALL, NULL);
    lv_test_mouse_click_at(sw->coords.x1, sw->coords.y1);

    TEST_ASSERT_EQUAL(1, value_changed);
}

#endif

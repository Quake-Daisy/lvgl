/**
 * @file lv_spinbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_spinbox.h"
#if LV_USE_SPINBOX

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS    &lv_spinbox_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_spinbox_constructor(lv_obj_t * obj, const lv_obj_t * copy);
static lv_res_t lv_spinbox_signal(lv_obj_t * obj, lv_signal_t sign, void * param);
static void lv_spinbox_updatevalue(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_spinbox_class = {
    .constructor_cb = lv_spinbox_constructor,
    .signal_cb = lv_spinbox_signal,
    .instance_size = sizeof(lv_spinbox_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .base_class = &lv_textarea_class
};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a spinbox object
 * @param par pointer to an object, it will be the parent of the new spinbox
 * @param copy pointer to a spinbox object, if not NULL then the new object will be copied from it
 * @return pointer to the created spinbox
 */
lv_obj_t * lv_spinbox_create(lv_obj_t * parent)
{
   return lv_obj_create_from_class(&lv_spinbox_class, parent, NULL);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set spinbox value
 * @param spinbox pointer to spinbox
 * @param i value to be set
 */
void lv_spinbox_set_value(lv_obj_t * obj, int32_t i)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    if(i > spinbox->range_max) i = spinbox->range_max;
    if(i < spinbox->range_min) i = spinbox->range_min;

    spinbox->value = i;

    lv_spinbox_updatevalue(obj);
}

/**
 * Set spinbox rollover function
 * @param spinbox pointer to spinbox
 * @param b true or false to enable or disable (default)
 */
void lv_spinbox_set_rollover(lv_obj_t * obj, bool b)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    spinbox->rollover = b;
}

/**
 * Set spinbox digit format (digit count and decimal format)
 * @param spinbox pointer to spinbox
 * @param digit_count number of digit excluding the decimal separator and the sign
 * @param separator_position number of digit before the decimal point. If 0, decimal point is not
 * shown
 */
void lv_spinbox_set_digit_format(lv_obj_t * obj, uint8_t digit_count, uint8_t separator_position)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    if(digit_count > LV_SPINBOX_MAX_DIGIT_COUNT) digit_count = LV_SPINBOX_MAX_DIGIT_COUNT;

    if(separator_position >= digit_count) separator_position = 0;
    if(separator_position > LV_SPINBOX_MAX_DIGIT_COUNT) separator_position = LV_SPINBOX_MAX_DIGIT_COUNT;

    if(digit_count < LV_SPINBOX_MAX_DIGIT_COUNT) {
        int64_t max_val = lv_pow(10, digit_count);
        if(spinbox->range_max > max_val - 1) spinbox->range_max = max_val - 1;
        if(spinbox->range_min < - max_val  + 1) spinbox->range_min = - max_val  + 1;
    }

    spinbox->digit_count   = digit_count;
    spinbox->dec_point_pos = separator_position;

    lv_spinbox_updatevalue(obj);
}

/**
 * Set spinbox step
 * @param spinbox pointer to spinbox
 * @param step steps on increment/decrement
 */
void lv_spinbox_set_step(lv_obj_t * obj, uint32_t step)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    spinbox->step = step;
    lv_spinbox_updatevalue(obj);
}

/**
 * Set spinbox value range
 * @param spinbox pointer to spinbox
 * @param range_min maximum value, inclusive
 * @param range_max minimum value, inclusive
 */
void lv_spinbox_set_range(lv_obj_t * obj, int32_t range_min, int32_t range_max)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    spinbox->range_max = range_max;
    spinbox->range_min = range_min;

    if(spinbox->value > spinbox->range_max) spinbox->value = spinbox->range_max;
    if(spinbox->value < spinbox->range_min) spinbox->value = spinbox->range_min;

    lv_spinbox_updatevalue(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the spinbox numeral value (user has to convert to float according to its digit format)
 * @param spinbox pointer to spinbox
 * @return value integer value of the spinbox
 */
int32_t lv_spinbox_get_value(lv_obj_t * obj)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    return spinbox->value;
}
/**
 * Get the spinbox step value (user has to convert to float according to its digit format)
 * @param spinbox pointer to spinbox
 * @return value integer step value of the spinbox
 */
int32_t lv_spinbox_get_step(lv_obj_t * obj)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    return spinbox->step;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Select next lower digit for edition
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_next(lv_obj_t * obj)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    int32_t new_step = spinbox->step / 10;
    if((new_step) > 0)
        spinbox->step = new_step;
    else
        spinbox->step = 1;

    lv_spinbox_updatevalue(obj);
}

/**
 * Select next higher digit for edition
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_prev(lv_obj_t * obj)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;
    int32_t step_limit;
    step_limit       = LV_MAX(spinbox->range_max, (spinbox->range_min < 0 ? (-spinbox->range_min) : spinbox->range_min));
    int32_t new_step = spinbox->step * 10;
    if(new_step <= step_limit) spinbox->step = new_step;

    lv_spinbox_updatevalue(obj);
}

/**
 * Get spinbox rollover function status
 * @param spinbox pointer to spinbox
 */
bool lv_spinbox_get_rollover(lv_obj_t * obj)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    return spinbox->rollover;
}

/**
 * Increment spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_increment(lv_obj_t * obj)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    if(spinbox->value + spinbox->step <= spinbox->range_max) {
        /*Special mode when zero crossing*/
        if((spinbox->value + spinbox->step) > 0 && spinbox->value < 0) spinbox->value = -spinbox->value;
        spinbox->value += spinbox->step;

    }
    else {
        // Rollover?
        if((spinbox->rollover) && (spinbox->value == spinbox->range_max))
            spinbox->value = spinbox->range_min;
        else
            spinbox->value = spinbox->range_max;
    }

    lv_spinbox_updatevalue(obj);
}

/**
 * Decrement spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_decrement(lv_obj_t * obj)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    if(spinbox->value - spinbox->step >= spinbox->range_min) {
        /*Special mode when zero crossing*/
        if((spinbox->value - spinbox->step) < 0 && spinbox->value > 0) spinbox->value = -spinbox->value;
        spinbox->value -= spinbox->step;
    }
    else {
        /*Rollover?*/
        if((spinbox->rollover) && (spinbox->value == spinbox->range_min))
            spinbox->value = spinbox->range_max;
        else
            spinbox->value = spinbox->range_min;
    }

    lv_spinbox_updatevalue(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_spinbox_constructor(lv_obj_t * obj, const lv_obj_t * copy)
{
    LV_UNUSED(copy);

    LV_LOG_TRACE("begin");

    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    /*Initialize the allocated 'ext'*/
    spinbox->value              = 0;
    spinbox->dec_point_pos      = 0;
    spinbox->digit_count        = 5;
    spinbox->step               = 1;
    spinbox->range_max          = 99999;
    spinbox->range_min          = -99999;
    spinbox->rollover           = false;

    lv_textarea_set_one_line(obj, true);
    lv_textarea_set_cursor_click_pos(obj, true);
    lv_obj_set_width(obj, LV_DPI_DEF);

    lv_spinbox_updatevalue(obj);

    LV_LOG_TRACE("Spinbox constructor finished");
}

static lv_res_t lv_spinbox_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    /* Include the ancient signal function */
    lv_res_t res = LV_RES_OK;
    res = lv_obj_signal_base(MY_CLASS, obj, sign, param);
    if(res != LV_RES_OK) return res;

    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;
    if(sign == LV_SIGNAL_RELEASED) {
        /*If released with an ENCODER then move to the next digit*/
        lv_indev_t * indev = lv_indev_get_act();
        if(lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
            if(lv_group_get_editing(lv_obj_get_group(obj))) {
                if(spinbox->step > 1) {
                    lv_spinbox_step_next(obj);
                }
                else {
                    /*Restart from the MSB*/
                    spinbox->step = 1;
                    uint32_t i;
                    for(i = 0; i < spinbox->digit_count; i++) {
                        int32_t new_step = spinbox->step * 10;
                        if(new_step >= spinbox->range_max) break;
                        spinbox->step = new_step;
                    }
                    lv_spinbox_step_prev(obj);
                }
            }
        }
        /*The cursor has been positioned to a digit.
         * Set `step` accordingly*/
        else {
            const char * txt = lv_textarea_get_text(obj);
            size_t txt_len = strlen(txt);

            if(txt[spinbox->ta.cursor.pos] == '.') {
                lv_textarea_cursor_left(obj);
            }
            else if(spinbox->ta.cursor.pos == (uint32_t)txt_len) {
                lv_textarea_set_cursor_pos(obj, txt_len - 1);
            }
            else if(spinbox->ta.cursor.pos == 0 && spinbox->range_min < 0) {
                lv_textarea_set_cursor_pos(obj, 1);
            }

            size_t len = spinbox->digit_count - 1;
            uint16_t cp = spinbox->ta.cursor.pos;

            if(spinbox->ta.cursor.pos > spinbox->dec_point_pos && spinbox->dec_point_pos != 0) cp--;
            uint32_t pos = len - cp;

            if(spinbox->range_min < 0) pos++;

            spinbox->step = 1;
            uint16_t i;
            for(i = 0; i < pos; i++) spinbox->step *= 10;
        }
    }
    else if(sign == LV_SIGNAL_CONTROL) {
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        uint32_t c = *((uint32_t *)param); /*uint32_t because can be UTF-8*/
        if(c == LV_KEY_RIGHT) {
            if(indev_type == LV_INDEV_TYPE_ENCODER)
                lv_spinbox_increment(obj);
            else
                lv_spinbox_step_next(obj);
        }
        else if(c == LV_KEY_LEFT) {
            if(indev_type == LV_INDEV_TYPE_ENCODER)
                lv_spinbox_decrement(obj);
            else
                lv_spinbox_step_prev(obj);
        }
        else if(c == LV_KEY_UP) {
            lv_spinbox_increment(obj);
        }
        else if(c == LV_KEY_DOWN) {
            lv_spinbox_decrement(obj);
        }
        else {
            lv_textarea_add_char(obj, c);
        }
    }

    return res;
}

static void lv_spinbox_updatevalue(lv_obj_t * obj)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *) obj;

    char buf[LV_SPINBOX_MAX_DIGIT_COUNT + 8];
    lv_memset_00(buf, sizeof(buf));
    char * buf_p = buf;
    uint8_t cur_shift_left = 0;

    if(spinbox->range_min < 0) {  // hide sign if there are only positive values
        /*Add the sign*/
        (*buf_p) = spinbox->value >= 0 ? '+' : '-';
        buf_p++;
    }
    else {
        /*Cursor need shift to left*/
        cur_shift_left++;
    }

    int32_t i;
    char digits[LV_SPINBOX_MAX_DIGIT_COUNT + 4];
    /*Convert the numbers to string (the sign is already handled so always covert positive number)*/
    lv_snprintf(digits, sizeof(digits), "%d", LV_ABS(spinbox->value));

    /*Add leading zeros*/
    int lz_cnt = spinbox->digit_count - (int)strlen(digits);
    if(lz_cnt > 0) {
        for(i = (uint16_t)strlen(digits); i >= 0; i--) {
            digits[i + lz_cnt] = digits[i];
        }
        for(i = 0; i < lz_cnt; i++) {
            digits[i] = '0';
        }
    }

    int32_t intDigits;
    intDigits = (spinbox->dec_point_pos == 0) ? spinbox->digit_count : spinbox->dec_point_pos;

    /*Add the decimal part*/
    for(i = 0; i < intDigits && digits[i] != '\0'; i++) {
        (*buf_p) = digits[i];
        buf_p++;
    }

    if(spinbox->dec_point_pos != 0) {
        /*Insert the decimal point*/
        (*buf_p) = '.';
        buf_p++;

        for(/*Leave i*/; i < spinbox->digit_count && digits[i] != '\0'; i++) {
            (*buf_p) = digits[i];
            buf_p++;
        }
    }

    /*Refresh the text*/
    lv_textarea_set_text(obj, (char *)buf);

    /*Set the cursor position*/
    int32_t step    = spinbox->step;
    uint8_t cur_pos = (uint8_t)spinbox->digit_count;
    while(step >= 10) {
        step /= 10;
        cur_pos--;
    }

    if(cur_pos > intDigits) cur_pos++; /*Skip the decimal point*/

    cur_pos -= cur_shift_left;

    lv_textarea_set_cursor_pos(obj, cur_pos);
}

#endif /*LV_USE_SPINBOX*/

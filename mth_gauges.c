#ifdef __cplusplus
extern "C" {
#endif

#include "mth_gauges.h"

static lv_obj_t *meter;
static lv_meter_indicator_t *indic = NULL;

static void set_value(void *indic_ptr, int32_t v)
{
    lv_meter_set_indicator_value(meter, indic_ptr, v);
}

/**
 * Set the speed value of the dark speedometer gauge (0..240).
 * Call this from loop() to update the gauge value directly (replaces animation).
 */
void mth_gauge_set_speed(int32_t v)
{
    if (meter && indic) {
        lv_meter_set_indicator_value(meter, indic, v);
    }
}

/**
 * A simple dark-themed speedometer meter example.
 */
void mth_gauge_speedometer_dark(void)
{
    /* 1. Create Meter & Set Dark Theme Base */
    meter = lv_meter_create(lv_scr_act());
    lv_obj_center(meter);

    // Increased the size slightly because we are pushing labels out
    int32_t size = (EXAMPLE_LCD_H_RES < EXAMPLE_LCD_V_RES)
                        ? EXAMPLE_LCD_H_RES - 30
                        : EXAMPLE_LCD_V_RES - 30;
    lv_obj_set_size(meter, size, size);

    lv_obj_set_style_bg_color(meter, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_width(meter, 0, 0);
    lv_obj_set_style_outline_width(meter, 0, 0);
    lv_obj_set_style_radius(meter, LV_RADIUS_CIRCLE, 0);

    /* 2. Configure Scale */
    lv_meter_scale_t *scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale, 0, 240, 240, 150);

    /* Minor Ticks: Thinner (1px) and shorter (8px) to reduce visual clutter */
    lv_meter_set_scale_ticks(meter, scale, 49, 1, 8, lv_color_hex(0x666666));

    /* Major Ticks:
        - Tick length reduced to 18px (from 22) to give the font more room
        - LABEL GAP increased to 35 (from 20) to push numbers outward
    */
    lv_meter_set_scale_major_ticks(meter, scale, 4, 3, 18, lv_color_hex(0xFFFFFF), 29);

    /* Font Style: Montserrat 24 */
    lv_obj_set_style_text_color(meter, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(meter, &lv_font_montserrat_24, 0);

    /* 3. The Needle - VIBRANT ORANGE */
    // Offset changed to -20 so it reaches the end of the scale properly
    indic = lv_meter_add_needle_line(meter, scale, 5, lv_color_hex(0xFF8C00), -20);

    /* 4. Pivot Point Decor */
    lv_obj_t *center_knob = lv_obj_create(meter);
    lv_obj_set_size(center_knob, size / 10, size / 10);
    lv_obj_center(center_knob);
    lv_obj_set_style_bg_color(center_knob, lv_color_hex(0x151515), 0);
    lv_obj_set_style_radius(center_knob, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_color(center_knob, lv_color_hex(0x444444), 0);
    lv_obj_set_style_border_width(center_knob, 2, 0);

    // 5. Animation removed - call mth_gauge_set_speed() from loop() to update the needle value 
    // lv_anim_t a;
    // lv_anim_init(&a);
    // lv_anim_set_exec_cb(&a, set_value);
    // lv_anim_set_var(&a, indic);
    // lv_anim_set_values(&a, 0, 240);
    // lv_anim_set_time(&a, 3000);
    // lv_anim_set_playback_time(&a, 1000);
    // lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    // lv_anim_start(&a);
}

#ifdef __cplusplus
}
#endif
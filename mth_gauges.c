#ifdef __cplusplus
extern "C" {
#endif

#include "mth_gauges.h"

static lv_obj_t *meter_t;
static lv_meter_indicator_t *indic_t;

// Vibrant Orange from your first example
#define COLOR_NEEDLE   lv_color_hex(0xFF8C00)
#define COLOR_MINOR    lv_color_hex(0x666666)
#define COLOR_MAJOR    lv_color_hex(0xFFFFFF)
#define COLOR_RED      lv_color_hex(0xFF0000)

void mth_gauge_set_temp(int32_t v) {
    if (meter_t && indic_t) lv_meter_set_indicator_value(meter_t, indic_t, v);
}

void mth_gauge_oil_temp_init(void) {
    // Use full screen - use the larger dimension to fill the screen
    int32_t size = (EXAMPLE_LCD_H_RES > EXAMPLE_LCD_V_RES) ? EXAMPLE_LCD_H_RES : EXAMPLE_LCD_V_RES;

    meter_t = lv_meter_create(lv_scr_act());
    lv_obj_set_size(meter_t, size, size);
    lv_obj_center(meter_t);

    lv_obj_set_style_bg_opa(meter_t, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(meter_t, 0, 0);
    lv_obj_set_style_text_color(meter_t, COLOR_MAJOR, 0);
    lv_obj_set_style_text_font(meter_t, &lv_font_montserrat_14, 0);

    lv_meter_scale_t *scale = lv_meter_add_scale(meter_t);

    /* OIL TEMP: 270-degree gauge
     * Range 0-180 Celsius.
     * Arc length: 270°, Rotation: 135° (starts at bottom, ends at right)
     * 0° at bottom, 180° at right side
     */
    lv_meter_set_scale_range(meter_t, scale, 0, 180, 270, 135);
    lv_meter_set_scale_ticks(meter_t, scale, 37, 2, 10, COLOR_MINOR);
    lv_meter_set_scale_major_ticks(meter_t, scale, 6, 3, 18, COLOR_MAJOR, 15);

    // Red zone indicator (135 to 180)
    lv_meter_indicator_t *red_arc = lv_meter_add_arc(meter_t, scale, 8, COLOR_RED, -2);
    lv_meter_set_indicator_start_value(meter_t, red_arc, 135);
    lv_meter_set_indicator_end_value(meter_t, red_arc, 180);

    indic_t = lv_meter_add_needle_line(meter_t, scale, 4, COLOR_NEEDLE, -15);
}

#ifdef __cplusplus
}
#endif
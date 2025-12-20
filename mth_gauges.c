#ifdef __cplusplus
extern "C" {
#endif

#include "mth_gauges.h"
#include <stdio.h>
#include <math.h>

// Global components
static lv_obj_t *temp_arc;
static lv_obj_t *temp_label;

// Color palette
#define COLOR_AMBER   lv_color_hex(0xFA8C00)
#define COLOR_BLACK   lv_color_hex(0x000000)
#define COLOR_RED     lv_color_hex(0xFF0000)
#define COLOR_WHITE   lv_color_hex(0xFFFFFF)

// Attributes
const int dimension = (EXAMPLE_LCD_H_RES > EXAMPLE_LCD_V_RES) ? EXAMPLE_LCD_H_RES : EXAMPLE_LCD_V_RES;
const int temp_min = 60;
const int temp_max = 160;
const int temp_redline = 135;
const int temp_arc_width = 24;
const int temp_line_width = 4;
const int temp_arc_size = dimension - (temp_line_width * 12);
const int arc_start = 135;
const int arc_end = 405;  // 135 + 270
const float marker_gap = ((float)(arc_end - arc_start)) / ((temp_max - temp_min) / 20);

// Helper functions
static void arc_anim_cb(void *arc, int32_t value) {
    lv_arc_set_value((lv_obj_t *)arc, value);
}

static void position_markers(lv_obj_t *marker, int position) {
    lv_obj_set_size(marker, temp_arc_size + (temp_line_width * 4), temp_arc_size + (temp_line_width * 4));
    lv_obj_align(marker, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_arc_width(marker, temp_arc_width + (temp_line_width * 4), LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(marker, false, LV_PART_MAIN);
    lv_obj_set_style_arc_color(marker, COLOR_WHITE, LV_PART_MAIN);

    lv_arc_set_bg_angles(marker, arc_start + (position * marker_gap), arc_start + (position * marker_gap) + 1);

    lv_obj_remove_style(marker, NULL, LV_PART_KNOB);
    lv_obj_remove_style(marker, NULL, LV_PART_INDICATOR);
}

void mth_gauge_set_temp(int32_t v) {
    if (!temp_arc) return;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, temp_arc);
    lv_anim_set_values(&a, lv_arc_get_value(temp_arc), v);
    lv_anim_set_time(&a, 200);
    lv_anim_set_exec_cb(&a, arc_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    if (v >= temp_redline) {
        lv_obj_set_style_arc_color(temp_arc, COLOR_RED, LV_PART_INDICATOR);
    } else {
        lv_obj_set_style_arc_color(temp_arc, COLOR_AMBER, LV_PART_INDICATOR);
    }

    static char temp_text[8];
    snprintf(temp_text, sizeof(temp_text), "%d", v);
    lv_label_set_text(temp_label, temp_text);
}

static void make_temp_arc(void) {
    temp_arc = lv_arc_create(lv_scr_act());

    lv_obj_set_size(temp_arc, temp_arc_size, temp_arc_size);
    lv_obj_align(temp_arc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(temp_arc, arc_start + 2, arc_end - 1);

    lv_obj_set_style_arc_color(temp_arc, COLOR_BLACK, LV_PART_MAIN);
    lv_obj_set_style_arc_color(temp_arc, COLOR_AMBER, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(temp_arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(temp_arc, temp_arc_width, LV_PART_INDICATOR);

    lv_obj_remove_style(temp_arc, NULL, LV_PART_KNOB);

    lv_arc_set_range(temp_arc, temp_min, temp_max);
    lv_arc_set_value(temp_arc, temp_min);
}

static void make_temp_border(void) {
    lv_obj_t *temp_arc_border = lv_arc_create(lv_scr_act());

    lv_obj_set_size(temp_arc_border, temp_arc_size - (temp_arc_width * 2) - (temp_line_width * 2),
                    temp_arc_size - (temp_arc_width * 2) - (temp_line_width * 2));
    lv_obj_align(temp_arc_border, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(temp_arc_border, arc_start, arc_end);

    lv_obj_set_style_arc_width(temp_arc_border, temp_line_width, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(temp_arc_border, false, LV_PART_MAIN);
    lv_obj_set_style_arc_color(temp_arc_border, COLOR_WHITE, LV_PART_MAIN);

    lv_obj_remove_style(temp_arc_border, NULL, LV_PART_KNOB);
    lv_obj_remove_style(temp_arc_border, NULL, LV_PART_INDICATOR);

    // Create markers every 20 degrees (60, 80, 100, 120, 140, 160)
    int temp_marker_count = ((temp_max - temp_min) / 20) + 1;

    // Style for marker labels
    static lv_style_t style_marker_text;
    lv_style_init(&style_marker_text);
    lv_style_set_text_font(&style_marker_text, &lv_font_montserrat_24);
    lv_style_set_text_color(&style_marker_text, COLOR_WHITE);

    for (int i = 0; i < temp_marker_count; i++) {
        lv_obj_t *marker = lv_arc_create(lv_scr_act());
        position_markers(marker, i);

        // Calculate position for text label
        int temp_value = temp_min + (i * 20);
        float angle_rad = (arc_start + (i * marker_gap)) * 3.14159f / 180.0f;
        int radius = (temp_arc_size / 2) - temp_arc_width - 40;
        int label_x = (int)(radius * cosf(angle_rad));
        int label_y = (int)(radius * sinf(angle_rad));

        // Create text label
        lv_obj_t *label = lv_label_create(lv_scr_act());
        char text[4];
        snprintf(text, sizeof(text), "%d", temp_value);
        lv_label_set_text(label, text);
        lv_obj_add_style(label, &style_marker_text, 0);
        lv_obj_align(label, LV_ALIGN_CENTER, label_x, label_y);
    }
}

static void make_temp_redline(void) {
    lv_obj_t *temp_redline_marker = lv_arc_create(lv_scr_act());

    lv_obj_set_size(temp_redline_marker, dimension, dimension);
    lv_obj_align(temp_redline_marker, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(
        temp_redline_marker,
        arc_start + ((arc_end - arc_start) * ((float)(temp_redline - temp_min) / (temp_max - temp_min))),
        arc_end
    );

    lv_obj_set_style_arc_color(temp_redline_marker, COLOR_RED, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(temp_redline_marker, false, LV_PART_MAIN);
    lv_obj_set_style_arc_width(temp_redline_marker, temp_line_width * 2, LV_PART_MAIN);

    lv_obj_remove_style(temp_redline_marker, NULL, LV_PART_KNOB);
    lv_obj_remove_style(temp_redline_marker, NULL, LV_PART_INDICATOR);
}

static void make_temp_digital(void) {
    static lv_style_t style_temp_text;
    lv_style_init(&style_temp_text);
    lv_style_set_text_font(&style_temp_text, &lv_font_montserrat_48);
    lv_style_set_text_color(&style_temp_text, COLOR_WHITE);

    temp_label = lv_label_create(lv_scr_act());
    lv_label_set_text(temp_label, "0");
    lv_obj_add_style(temp_label, &style_temp_text, 0);
    lv_obj_align(temp_label, LV_ALIGN_CENTER, 0, -10);

    static lv_style_t style_unit_text;
    lv_style_init(&style_unit_text);
    lv_style_set_text_font(&style_unit_text, &lv_font_montserrat_24);
    lv_style_set_text_color(&style_unit_text, COLOR_AMBER);

    lv_obj_t *temp_unit_label = lv_label_create(lv_scr_act());
    lv_label_set_text(temp_unit_label, "°C");
    lv_obj_add_style(temp_unit_label, &style_unit_text, 0);
    lv_obj_align(temp_unit_label, LV_ALIGN_CENTER, 0, 30);
}

void mth_gauge_oil_temp_init(void) {
    make_temp_arc();
    make_temp_border();
    make_temp_redline();
    make_temp_digital();
}

#ifdef __cplusplus
}
#endif
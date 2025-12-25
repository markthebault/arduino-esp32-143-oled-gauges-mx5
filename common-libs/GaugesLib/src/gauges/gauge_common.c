#ifdef __cplusplus
extern "C" {
#endif

#include "gauge_common.h"
#include <stdio.h>
#include <math.h>

// Include custom font definitions (only in this compilation unit)
#if USE_CUSTOM_TEMP_FONT
    #include "../fonts/montserrat_bold_numbers_120.h"
#endif

#if USE_CUSTOM_ICON_FONT
    #ifdef USE_SCREEN_466PX
        #include "../fonts/fa_icons_54.h"
    #endif
    #ifdef USE_SCREEN_240PX
        #include "../fonts/fa_icons_32.h"
    #endif
#endif

// ============================================================================
// ANIMATION CALLBACKS
// ============================================================================

void gauge_arc_anim_cb(void *arc, int32_t value) {
    lv_arc_set_value((lv_obj_t *)arc, value);
}

void gauge_bg_blink_anim_cb(void *obj, int32_t value) {
    if (value == 0) {
        lv_obj_set_style_bg_color((lv_obj_t *)obj, COLOR_BLACK, LV_PART_MAIN);
    } else {
        lv_obj_set_style_bg_color((lv_obj_t *)obj, COLOR_RED, LV_PART_MAIN);
    }
}

// ============================================================================
// ANIMATION CONTROL
// ============================================================================

void gauge_start_blink(gauge_state_t *state) {
    if (!state || !state->screen_bg) return;

    lv_anim_init(&state->blink_anim);
    lv_anim_set_var(&state->blink_anim, state->screen_bg);
    lv_anim_set_values(&state->blink_anim, 0, 1);
    lv_anim_set_time(&state->blink_anim, BLINK_ANIM_TIME);
    lv_anim_set_playback_time(&state->blink_anim, BLINK_PLAYBACK_TIME);
    lv_anim_set_repeat_count(&state->blink_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&state->blink_anim, gauge_bg_blink_anim_cb);
    lv_anim_set_path_cb(&state->blink_anim, lv_anim_path_step);
    lv_anim_start(&state->blink_anim);
}

void gauge_stop_blink(gauge_state_t *state) {
    if (!state || !state->screen_bg) return;

    lv_anim_del(state->screen_bg, NULL);
    lv_obj_set_style_bg_color(state->screen_bg, COLOR_BLACK, LV_PART_MAIN);
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

float gauge_calc_marker_gap(int32_t temp_min, int32_t temp_max, int32_t marker_interval) {
    return ((float)(ARC_END_ANGLE - ARC_START_ANGLE)) /
           ((temp_max - temp_min) / marker_interval);
}

static void position_marker(lv_obj_t *marker, int position, float marker_gap) {
    lv_obj_set_size(marker, ARC_SIZE + (LINE_WIDTH * ARC_MARKER_SIZE_MULTIPLIER),
                    ARC_SIZE + (LINE_WIDTH * ARC_MARKER_SIZE_MULTIPLIER));
    lv_obj_align(marker, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_arc_width(marker, ARC_WIDTH + (LINE_WIDTH * ARC_MARKER_SIZE_MULTIPLIER), LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(marker, false, LV_PART_MAIN);
    lv_obj_set_style_arc_color(marker, COLOR_WHITE, LV_PART_MAIN);

    lv_arc_set_bg_angles(marker,
                        ARC_START_ANGLE + (position * marker_gap),
                        ARC_START_ANGLE + (position * marker_gap) + ARC_MARKER_ANGLE_OFFSET);

    lv_obj_remove_style(marker, NULL, LV_PART_KNOB);
    lv_obj_remove_style(marker, NULL, LV_PART_INDICATOR);
}

// ============================================================================
// UI COMPONENT CREATION
// ============================================================================

lv_obj_t* gauge_create_background(void) {
    lv_obj_t *screen_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(screen_bg, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen_bg, COLOR_BLACK, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen_bg, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(screen_bg, 0, LV_PART_MAIN);
    lv_obj_move_background(screen_bg);
    return screen_bg;
}

lv_obj_t* gauge_create_arc(const gauge_config_t *config) {
    lv_obj_t *arc = lv_arc_create(lv_scr_act());

    lv_obj_set_size(arc, ARC_SIZE, ARC_SIZE);
    lv_obj_align(arc, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(arc, ARC_START_ANGLE + ARC_ANGLE_OFFSET_START, ARC_END_ANGLE - ARC_ANGLE_OFFSET_END);

    lv_obj_set_style_arc_color(arc, COLOR_BLACK, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, COLOR_AMBER, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, ARC_WIDTH, LV_PART_INDICATOR);

    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);

    lv_arc_set_range(arc, config->temp_min, config->temp_max);
    lv_arc_set_value(arc, config->temp_min);

    return arc;
}

void gauge_create_border_and_markers(const gauge_config_t *config) {
    // Create border arc
    lv_obj_t *border = lv_arc_create(lv_scr_act());

    lv_obj_set_size(border,
                    ARC_SIZE - (ARC_WIDTH * ARC_BORDER_WIDTH_MULTIPLIER) - (LINE_WIDTH * ARC_BORDER_LINE_MULTIPLIER),
                    ARC_SIZE - (ARC_WIDTH * ARC_BORDER_WIDTH_MULTIPLIER) - (LINE_WIDTH * ARC_BORDER_LINE_MULTIPLIER));
    lv_obj_align(border, LV_ALIGN_CENTER, 0, 0);
    lv_arc_set_bg_angles(border, ARC_START_ANGLE, ARC_END_ANGLE);

    lv_obj_set_style_arc_width(border, LINE_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(border, false, LV_PART_MAIN);
    lv_obj_set_style_arc_color(border, COLOR_WHITE, LV_PART_MAIN);

    lv_obj_remove_style(border, NULL, LV_PART_KNOB);
    lv_obj_remove_style(border, NULL, LV_PART_INDICATOR);

    // Calculate markers
    int marker_count = ((config->temp_max - config->temp_min) / config->marker_interval) + 1;
    float marker_gap = gauge_calc_marker_gap(config->temp_min, config->temp_max,
                                             config->marker_interval);

    // Style for marker labels
    static lv_style_t style_marker_text;
    lv_style_init(&style_marker_text);
    lv_style_set_text_font(&style_marker_text, FONT_MARKERS);
    lv_style_set_text_color(&style_marker_text, COLOR_WHITE);

    // Create markers and labels
    for (int i = 0; i < marker_count; i++) {
        // Create marker tick
        lv_obj_t *marker = lv_arc_create(lv_scr_act());
        position_marker(marker, i, marker_gap);

        // Calculate label position
        int temp_value = config->temp_min + (i * config->marker_interval);
        float angle_rad = (ARC_START_ANGLE + (i * marker_gap)) * MATH_PI / DEGREES_TO_RADIANS;
        int radius = (ARC_SIZE / 2) - ARC_WIDTH - MARKER_LABEL_OFFSET;
        int label_x = (int)(radius * cosf(angle_rad));
        int label_y = (int)(radius * sinf(angle_rad));

        // Create label
        lv_obj_t *label = lv_label_create(lv_scr_act());
        char text[8];

        // Format text based on value scale
        if (config->value_scale > 1) {
            // Show decimal value (e.g., for pressure: 10 -> 1.0)
            int scaled_value = temp_value / config->value_scale;
            snprintf(text, sizeof(text), "%d", scaled_value);
        } else {
            // Show integer value (e.g., for temperature: 80 -> 80)
            snprintf(text, sizeof(text), "%d", temp_value);
        }

        lv_label_set_text(label, text);
        lv_obj_add_style(label, &style_marker_text, 0);
        lv_obj_align(label, LV_ALIGN_CENTER, label_x, label_y);
    }
}

void gauge_create_redline(const gauge_config_t *config) {
    lv_obj_t *redline = lv_arc_create(lv_scr_act());

    lv_obj_set_size(redline, GAUGE_DIMENSION, GAUGE_DIMENSION);
    lv_obj_align(redline, LV_ALIGN_CENTER, 0, 0);

    float redline_angle = ARC_START_ANGLE +
                         ((ARC_END_ANGLE - ARC_START_ANGLE) *
                          ((float)(config->redline - config->temp_min) /
                           (config->temp_max - config->temp_min)));

    lv_arc_set_bg_angles(redline, redline_angle, ARC_END_ANGLE);

    lv_obj_set_style_arc_color(redline, COLOR_RED, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(redline, false, LV_PART_MAIN);
    lv_obj_set_style_arc_width(redline, LINE_WIDTH * ARC_REDLINE_WIDTH_MULTIPLIER, LV_PART_MAIN);

    lv_obj_remove_style(redline, NULL, LV_PART_KNOB);
    lv_obj_remove_style(redline, NULL, LV_PART_INDICATOR);
}

lv_obj_t* gauge_create_digital_display(void) {
    // Create main temperature text
    static lv_style_t style_temp_text;
    lv_style_init(&style_temp_text);
    lv_style_set_text_font(&style_temp_text, FONT_TEMP_MAIN);
    lv_style_set_text_color(&style_temp_text, COLOR_WHITE);

    lv_obj_t *temp_label = lv_label_create(lv_scr_act());
    lv_label_set_text(temp_label, "0");
    lv_obj_add_style(temp_label, &style_temp_text, 0);
    lv_obj_align(temp_label, LV_ALIGN_CENTER, 0, TEMP_LABEL_Y_OFFSET);

    // Create unit text
    static lv_style_t style_unit_text;
    lv_style_init(&style_unit_text);
    lv_style_set_text_font(&style_unit_text, FONT_TEMP_UNIT);
    lv_style_set_text_color(&style_unit_text, COLOR_AMBER);

    lv_obj_t *temp_unit_label = lv_label_create(lv_scr_act());
    lv_label_set_text(temp_unit_label, "°C");
    lv_obj_add_style(temp_unit_label, &style_unit_text, 0);
    lv_obj_align(temp_unit_label, LV_ALIGN_CENTER, 0, TEMP_UNIT_Y_OFFSET);

    return temp_label;
}

lv_obj_t* gauge_create_icon(const char *icon_symbol) {
    static lv_style_t style_icon;
    lv_style_init(&style_icon);
    lv_style_set_text_font(&style_icon, FONT_ICON);
    lv_style_set_text_color(&style_icon, COLOR_AMBER);

    lv_obj_t *icon = lv_label_create(lv_scr_act());

    #if USE_CUSTOM_ICON_FONT
        // Use Font Awesome icon symbol
        lv_label_set_text(icon, icon_symbol);
    #else
        // Use text fallback from global configuration
        if (icon_symbol == OIL_TEMP_SYMBOL) {
            lv_label_set_text(icon, OIL_TEMP_TEXT_LABEL);
        } else if (icon_symbol == OIL_PRESSURE_SYMBOL) {
            lv_label_set_text(icon, OIL_PRES_TEXT_LABEL);
        } else if (icon_symbol == WATER_SYMBOL) {
            lv_label_set_text(icon, WATER_TEXT_LABEL);
        } else {
            lv_label_set_text(icon, "---");
        }
    #endif

    lv_obj_add_style(icon, &style_icon, 0);
    lv_obj_align(icon, LV_ALIGN_BOTTOM_MID, 0, ICON_Y_OFFSET);

    return icon;
}

// ============================================================================
// GAUGE UPDATE
// ============================================================================

void gauge_update_value(gauge_state_t *state, const gauge_config_t *config, int32_t temperature) {
    if (!state || !state->arc || !state->label) return;

    // Store actual temperature for display
    int32_t display_temp = temperature;

    // Constrain temperature to gauge range for arc display
    int32_t arc_temp = temperature;
    if (arc_temp < config->temp_min) arc_temp = config->temp_min;
    if (arc_temp > config->temp_max) arc_temp = config->temp_max;

    // Animate arc to new value (constrained)
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, state->arc);
    lv_anim_set_values(&a, lv_arc_get_value(state->arc), arc_temp);
    lv_anim_set_time(&a, GAUGE_ANIM_TIME);
    lv_anim_set_exec_cb(&a, gauge_arc_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    // Set color based on temperature zones (using constrained value)
    if (arc_temp < config->zone_green) {
        lv_obj_set_style_arc_color(state->arc, COLOR_GREY, LV_PART_INDICATOR);
    } else if (arc_temp < config->zone_orange) {
        lv_obj_set_style_arc_color(state->arc, COLOR_GREEN, LV_PART_INDICATOR);
    } else if (arc_temp < config->zone_red) {
        lv_obj_set_style_arc_color(state->arc, COLOR_AMBER, LV_PART_INDICATOR);
    } else {
        lv_obj_set_style_arc_color(state->arc, COLOR_RED, LV_PART_INDICATOR);
    }

    // Handle alert animation (using actual temperature)
    if (display_temp >= config->alert_threshold && !state->is_blinking) {
        gauge_start_blink(state);
        state->is_blinking = true;
    } else if (display_temp < config->alert_threshold && state->is_blinking) {
        gauge_stop_blink(state);
        state->is_blinking = false;
    }

    // Update digital display with actual temperature (not constrained)
    static char temp_text[8];
    snprintf(temp_text, sizeof(temp_text), "%d", display_temp);
    lv_label_set_text(state->label, temp_text);
}

#ifdef __cplusplus
}
#endif

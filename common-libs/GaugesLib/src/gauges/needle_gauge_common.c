#ifdef __cplusplus
extern "C" {
#endif

#include "needle_gauge_common.h"
#include <stdio.h>
#include <math.h>

// Note: Custom fonts are already declared in gauge_common.c and gauges_config.h
// We don't need to include them again here to avoid multiple definition errors

// ============================================================================
// ANIMATION CALLBACKS
// ============================================================================

void needle_gauge_set_value_anim(void *indic, int32_t value) {
    lv_meter_set_indicator_value((lv_obj_t *)((void **)indic)[0],
                                  (lv_meter_indicator_t *)((void **)indic)[1],
                                  value);
}

void needle_gauge_bg_blink_cb(void *obj, int32_t value) {
    if (value == 0) {
        lv_obj_set_style_bg_color((lv_obj_t *)obj, COLOR_BLACK, LV_PART_MAIN);
    } else {
        lv_obj_set_style_bg_color((lv_obj_t *)obj, COLOR_RED, LV_PART_MAIN);
    }
}

// ============================================================================
// ANIMATION CONTROL
// ============================================================================

void needle_gauge_start_blink(needle_gauge_state_t *state) {
    if (!state || !state->screen_bg) return;

    lv_anim_init(&state->blink_anim);
    lv_anim_set_var(&state->blink_anim, state->screen_bg);
    lv_anim_set_values(&state->blink_anim, 0, 1);
    lv_anim_set_time(&state->blink_anim, BLINK_ANIM_TIME);
    lv_anim_set_playback_time(&state->blink_anim, BLINK_PLAYBACK_TIME);
    lv_anim_set_repeat_count(&state->blink_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&state->blink_anim, needle_gauge_bg_blink_cb);
    lv_anim_set_path_cb(&state->blink_anim, lv_anim_path_step);
    lv_anim_start(&state->blink_anim);
}

void needle_gauge_stop_blink(needle_gauge_state_t *state) {
    if (!state || !state->screen_bg) return;

    lv_anim_del(state->screen_bg, NULL);
    lv_obj_set_style_bg_color(state->screen_bg, COLOR_BLACK, LV_PART_MAIN);
}

// ============================================================================
// UI COMPONENT CREATION
// ============================================================================

lv_obj_t* needle_gauge_create_background(void) {
    lv_obj_t *screen_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(screen_bg, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(screen_bg, COLOR_BLACK, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen_bg, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(screen_bg, 0, LV_PART_MAIN);
    lv_obj_move_background(screen_bg);
    return screen_bg;
}

lv_obj_t* needle_gauge_create_meter(const needle_gauge_config_t *config, needle_gauge_state_t *state) {
    // Create meter widget
    lv_obj_t *meter = lv_meter_create(lv_scr_act());
    lv_obj_center(meter);
    lv_obj_set_size(meter, NEEDLE_METER_SIZE, NEEDLE_METER_SIZE);

    // Dark theme styling
    lv_obj_set_style_bg_color(meter, COLOR_BLACK, 0);
    lv_obj_set_style_border_width(meter, 0, 0);
    lv_obj_set_style_outline_width(meter, 0, 0);
    lv_obj_set_style_radius(meter, LV_RADIUS_CIRCLE, 0);

    // Font style for labels
    lv_obj_set_style_text_color(meter, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(meter, FONT_MARKERS, 0);

    // Add scale
    lv_meter_scale_t *scale = lv_meter_add_scale(meter);

    // Calculate range and angles
    int32_t range = (int32_t)(config->value_max - config->value_min);

    // Set scale range and rotation
    lv_meter_set_scale_range(meter, scale,
                             (int32_t)config->value_min,
                             (int32_t)config->value_max,
                             NEEDLE_ANGLE_RANGE,
                             NEEDLE_ANGLE_START);

    // Calculate tick counts
    // Minor ticks: one every unit (or every 0.5 for pressure)
    int32_t minor_tick_count;
    if (config->decimal_places > 0) {
        // For pressure (0-8 bar): 16 minor ticks (one every 0.5 bar)
        minor_tick_count = range * 2 + 1;
    } else {
        // For temperature: depends on range
        // For 60-160: 100 units = ~50 ticks (one every 2 units)
        minor_tick_count = range / 2 + 1;
    }

    // Minor ticks: thinner and shorter
    lv_meter_set_scale_ticks(meter, scale,
                            minor_tick_count,           // count
                            1,                          // width (1px)
                            (int)(8 * GAUGE_SCALE),    // length
                            lv_color_hex(0x666666));    // gray color

    // Major ticks with labels
    // Calculate how many minor ticks between each major tick
    int32_t ticks_between_major = (minor_tick_count - 1) / (config->major_tick_count - 1);

    lv_meter_set_scale_major_ticks(meter, scale,
                                   ticks_between_major,        // every nth tick
                                   3,                          // width
                                   (int)(18 * GAUGE_SCALE),   // length
                                   COLOR_WHITE,                // color
                                   (int)(29 * GAUGE_SCALE));  // label gap (distance from tick)

    // Add needle indicator (vibrant orange/amber)
    lv_meter_indicator_t *needle = lv_meter_add_needle_line(
        meter, scale,
        NEEDLE_WIDTH,           // width
        COLOR_AMBER,            // color
        (int)(-20 * GAUGE_SCALE) // offset to reach end of scale
    );

    // Create center knob decoration
    lv_obj_t *center_knob = lv_obj_create(meter);
    lv_obj_set_size(center_knob, NEEDLE_CENTER_SIZE, NEEDLE_CENTER_SIZE);
    lv_obj_center(center_knob);
    lv_obj_set_style_bg_color(center_knob, lv_color_hex(0x151515), 0);
    lv_obj_set_style_radius(center_knob, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_color(center_knob, lv_color_hex(0x444444), 0);
    lv_obj_set_style_border_width(center_knob, 2, 0);

    // Store in state
    state->meter = meter;
    state->scale = scale;
    state->needle_indicator = needle;

    // Set initial value
    lv_meter_set_indicator_value(meter, needle, (int32_t)config->value_min);

    return meter;
}

lv_obj_t* needle_gauge_create_value_label(const needle_gauge_config_t *config) {
    // Create value label (just the number, no unit)
    static lv_style_t style_value;
    lv_style_init(&style_value);
    lv_style_set_text_font(&style_value, FONT_TEMP_UNIT);
    lv_style_set_text_color(&style_value, COLOR_AMBER);

    lv_obj_t *value_label = lv_label_create(lv_scr_act());

    // Initialize with placeholder number only
    if (config->decimal_places > 0) {
        lv_label_set_text(value_label, "0.0");
    } else {
        lv_label_set_text(value_label, "0");
    }

    lv_obj_add_style(value_label, &style_value, 0);

    // Position in bottom right area, closer to center
    lv_obj_align(value_label, LV_ALIGN_CENTER,
                (int)(100 * GAUGE_SCALE),
                (int)(100 * GAUGE_SCALE));

    return value_label;
}

lv_obj_t* needle_gauge_create_unit_label(const needle_gauge_config_t *config) {
    // Create unit label with smaller font
    static lv_style_t style_unit;
    lv_style_init(&style_unit);
    lv_style_set_text_font(&style_unit, FONT_MARKERS);  // Smaller font for unit
    lv_style_set_text_color(&style_unit, COLOR_AMBER);

    lv_obj_t *unit_label = lv_label_create(lv_scr_act());

    // Set unit text
    if (config->unit_text) {
        lv_label_set_text(unit_label, config->unit_text);
    } else {
        lv_label_set_text(unit_label, "");
    }

    lv_obj_add_style(unit_label, &style_unit, 0);

    // Position next to value label (will be adjusted after value is set)
    lv_obj_align(unit_label, LV_ALIGN_CENTER,
                (int)(140 * GAUGE_SCALE),
                (int)(100 * GAUGE_SCALE));

    return unit_label;
}

lv_obj_t* needle_gauge_create_icon_label(const needle_gauge_config_t *config) {
    // Create icon/text label centered above gauge
    static lv_style_t style_icon;
    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, COLOR_AMBER);

    lv_obj_t *icon_label = lv_label_create(lv_scr_act());

    #if USE_CUSTOM_ICON_FONT
        // Use Font Awesome icon symbol
        lv_style_set_text_font(&style_icon, FONT_ICON);
        lv_label_set_text(icon_label, config->icon_symbol);
    #else
        // Use text fallback with smaller font
        lv_style_set_text_font(&style_icon, FONT_MARKERS);
        if (config->icon_symbol == OIL_TEMP_SYMBOL) {
            lv_label_set_text(icon_label, OIL_TEMP_TEXT_LABEL);
        } else if (config->icon_symbol == OIL_PRESSURE_SYMBOL) {
            lv_label_set_text(icon_label, OIL_PRES_TEXT_LABEL);
        } else if (config->icon_symbol == WATER_SYMBOL) {
            lv_label_set_text(icon_label, WATER_TEXT_LABEL);
        } else {
            lv_label_set_text(icon_label, "---");
        }
    #endif

    lv_obj_add_style(icon_label, &style_icon, 0);

    // Position at top of meter gauge
    int padding_offset_value = (int)15 / 2;
    lv_obj_align(icon_label, LV_ALIGN_CENTER,
                padding_offset_value,
                (int)(-80 * GAUGE_SCALE));

    return icon_label;
}

// ============================================================================
// NEEDLE GAUGE UPDATE
// ============================================================================

void needle_gauge_update_value(needle_gauge_state_t *state, const needle_gauge_config_t *config, float value) {
    if (!state || !state->meter || !state->needle_indicator) return;

    // Store actual raw value (not clamped)
    float actual_value = value;

    // Store current value for animation
    float old_value = state->current_value;
    state->current_value = value;

    // Clamp value to range ONLY for needle display
    float clamped_value = value;
    if (clamped_value < config->value_min) clamped_value = config->value_min;
    if (clamped_value > config->value_max) clamped_value = config->value_max;

    // Animate needle to new clamped value
    lv_anim_t a;
    lv_anim_init(&a);

    // Store both meter and indicator in an array for the animation callback
    static void *anim_vars[2];
    anim_vars[0] = state->meter;
    anim_vars[1] = state->needle_indicator;

    // Clamp old value for animation as well
    float old_clamped = old_value;
    if (old_clamped < config->value_min) old_clamped = config->value_min;
    if (old_clamped > config->value_max) old_clamped = config->value_max;

    lv_anim_set_var(&a, anim_vars);
    lv_anim_set_values(&a, (int32_t)old_clamped, (int32_t)clamped_value);
    lv_anim_set_time(&a, GAUGE_ANIM_TIME);
    lv_anim_set_exec_cb(&a, needle_gauge_set_value_anim);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    // Update needle color based on ACTUAL value zones (not clamped)
    lv_color_t needle_color;
    if (actual_value < config->zone_green) {
        needle_color = COLOR_GREY;
    } else if (actual_value < config->zone_orange) {
        needle_color = COLOR_GREEN;
    } else if (actual_value < config->zone_red) {
        needle_color = COLOR_AMBER;
    } else {
        needle_color = COLOR_RED;
    }

    // Update needle color
    lv_obj_set_style_line_color(state->meter, needle_color, LV_PART_ITEMS);

    // Handle alert animation based on ACTUAL value (not clamped)
    if (actual_value >= config->alert_threshold && !state->is_blinking) {
        needle_gauge_start_blink(state);
        state->is_blinking = true;
    } else if (actual_value < config->alert_threshold && state->is_blinking) {
        needle_gauge_stop_blink(state);
        state->is_blinking = false;
    }

    // Update value label with ACTUAL value (not clamped) - number only, no unit
    if (state->value_label) {
        char text[32];
        if (config->decimal_places > 0) {
            snprintf(text, sizeof(text), "%.*f",
                    config->decimal_places, actual_value);
        } else {
            snprintf(text, sizeof(text), "%d", (int)actual_value);
        }
        lv_label_set_text(state->value_label, text);
    }
}

#ifdef __cplusplus
}
#endif

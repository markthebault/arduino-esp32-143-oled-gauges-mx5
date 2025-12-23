#ifdef __cplusplus
extern "C" {
#endif

#include "oil_pressure_gauge.h"
#include "gauge_common.h"
#include <stdio.h>

// ============================================================================
// PRIVATE STATE
// ============================================================================

static gauge_state_t pressure_gauge_state = {0};

// Scale factor: multiply bar values by 10 for better arc resolution
// This means 0-8 bar becomes 0-80 internal units
#define PRESSURE_SCALE 10

static const gauge_config_t pressure_gauge_config = {
    .temp_min = (int32_t)(OIL_PRESSURE_MIN * PRESSURE_SCALE),
    .temp_max = (int32_t)(OIL_PRESSURE_MAX * PRESSURE_SCALE),
    .zone_green = (int32_t)(OIL_PRESSURE_ZONE_GREEN * PRESSURE_SCALE),
    .zone_orange = (int32_t)(OIL_PRESSURE_ZONE_ORANGE * PRESSURE_SCALE),
    .zone_red = (int32_t)(OIL_PRESSURE_ZONE_RED * PRESSURE_SCALE),
    .redline = (int32_t)(OIL_PRESSURE_REDLINE * PRESSURE_SCALE),
    .alert_threshold = (int32_t)(OIL_PRESSURE_ALERT_HIGH * PRESSURE_SCALE),  // High alert
    .marker_interval = 10,  // Markers every 1 bar (10 internal units)
    .icon_symbol = OIL_PRESSURE_SYMBOL
};

// Store current RPM for dynamic threshold calculation
static int32_t current_rpm = 0;

// ============================================================================
// PRIVATE FUNCTIONS
// ============================================================================

/**
 * @brief Calculate minimum safe oil pressure based on RPM
 * (Same formula as multi-gauge)
 */
static float calculate_min_oil_pressure(int32_t rpm) {
    float min_pressure = (float)rpm / 2000.0f;
    if (min_pressure < 0.5f) {
        min_pressure = 0.5f;
    }
    return min_pressure;
}

/**
 * @brief Update pressure gauge with custom logic for RPM-dependent thresholds
 */
static void oil_pressure_gauge_update_custom(float pressure, int32_t rpm) {
    if (!pressure_gauge_state.arc || !pressure_gauge_state.label) return;

    // Convert pressure to internal scaled value
    int32_t scaled_pressure = (int32_t)(pressure * PRESSURE_SCALE);

    // Animate arc to new value
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, pressure_gauge_state.arc);
    lv_anim_set_values(&a, lv_arc_get_value(pressure_gauge_state.arc), scaled_pressure);
    lv_anim_set_time(&a, GAUGE_ANIM_TIME);
    lv_anim_set_exec_cb(&a, gauge_arc_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);

    // Calculate dynamic minimum safe pressure
    float min_safe_pressure = calculate_min_oil_pressure(rpm);

    // Set color based on pressure zones with RPM-dependent low threshold
    if (pressure < min_safe_pressure) {
        // Too low for current RPM - RED
        lv_obj_set_style_arc_color(pressure_gauge_state.arc, COLOR_RED, LV_PART_INDICATOR);
    } else if (pressure >= OIL_PRESSURE_ZONE_RED) {
        // Too high - RED
        lv_obj_set_style_arc_color(pressure_gauge_state.arc, COLOR_RED, LV_PART_INDICATOR);
    } else if (pressure >= OIL_PRESSURE_ZONE_ORANGE) {
        // Getting high - AMBER
        lv_obj_set_style_arc_color(pressure_gauge_state.arc, COLOR_AMBER, LV_PART_INDICATOR);
    } else {
        // Good range - GREEN
        lv_obj_set_style_arc_color(pressure_gauge_state.arc, COLOR_GREEN, LV_PART_INDICATOR);
    }

    // Handle alert animation (both too low and too high are dangerous)
    bool should_alert = (pressure < min_safe_pressure) || (pressure >= OIL_PRESSURE_ALERT_HIGH);

    if (should_alert && !pressure_gauge_state.is_blinking) {
        gauge_start_blink(&pressure_gauge_state);
        pressure_gauge_state.is_blinking = true;
    } else if (!should_alert && pressure_gauge_state.is_blinking) {
        gauge_stop_blink(&pressure_gauge_state);
        pressure_gauge_state.is_blinking = false;
    }

    // Update digital display with one decimal place
    static char pressure_text[16];
    int whole = (int)pressure;
    int decimal = (int)((pressure - whole) * 10);
    snprintf(pressure_text, sizeof(pressure_text), "%d.%d", whole, decimal);
    lv_label_set_text(pressure_gauge_state.label, pressure_text);
}

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void oil_pressure_gauge_init(void) {
    // Initialize state
    pressure_gauge_state.is_blinking = false;

    // Create UI components using common utilities
    pressure_gauge_state.screen_bg = gauge_create_background();
    pressure_gauge_state.arc = gauge_create_arc(&pressure_gauge_config);
    gauge_create_border_and_markers(&pressure_gauge_config);
    gauge_create_redline(&pressure_gauge_config);
    pressure_gauge_state.label = gauge_create_digital_display();
    pressure_gauge_state.icon = gauge_create_icon(pressure_gauge_config.icon_symbol);
}

void oil_pressure_gauge_set_value(float pressure, int32_t rpm) {
    current_rpm = rpm;
    oil_pressure_gauge_update_custom(pressure, rpm);
}

#ifdef __cplusplus
}
#endif

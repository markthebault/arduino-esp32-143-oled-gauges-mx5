#ifdef __cplusplus
extern "C" {
#endif

#include "water_temp_gauge.h"
#include "gauge_common.h"

// ============================================================================
// PRIVATE STATE
// ============================================================================

static gauge_state_t water_gauge_state = {0};

static const gauge_config_t water_gauge_config = {
    .temp_min = WATER_TEMP_MIN,
    .temp_max = WATER_TEMP_MAX,
    .zone_green = WATER_TEMP_ZONE_GREEN,
    .zone_orange = WATER_TEMP_ZONE_ORANGE,
    .zone_red = WATER_TEMP_ZONE_RED,
    .redline = WATER_TEMP_REDLINE,
    .alert_threshold = WATER_TEMP_ALERT_THRESHOLD,
    .marker_interval = WATER_TEMP_MARKER_INTERVAL,
    .icon_symbol = WATER_SYMBOL,
    .value_scale = 1  // No scaling for temperature
};

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void water_temp_gauge_init(void) {
    // Initialize state
    water_gauge_state.is_blinking = false;

    // Create UI components using common utilities
    water_gauge_state.screen_bg = gauge_create_background();
    water_gauge_state.arc = gauge_create_arc(&water_gauge_config);
    gauge_create_border_and_markers(&water_gauge_config);
    gauge_create_redline(&water_gauge_config);
    water_gauge_state.label = gauge_create_digital_display();
    water_gauge_state.icon = gauge_create_icon(water_gauge_config.icon_symbol);
}

void water_temp_gauge_set_value(int32_t temperature) {
    gauge_update_value(&water_gauge_state, &water_gauge_config, temperature);
}

#ifdef __cplusplus
}
#endif

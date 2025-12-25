#ifdef __cplusplus
extern "C" {
#endif

#include "water_temp_needle_gauge.h"
#include "needle_gauge_common.h"

// ============================================================================
// PRIVATE STATE
// ============================================================================

static needle_gauge_state_t water_needle_state = {0};

static const needle_gauge_config_t water_needle_config = {
    .value_min = (float)WATER_TEMP_MIN,
    .value_max = (float)WATER_TEMP_MAX,
    .zone_green = (float)WATER_TEMP_ZONE_GREEN,
    .zone_orange = (float)WATER_TEMP_ZONE_ORANGE,
    .zone_red = (float)WATER_TEMP_ZONE_RED,
    .alert_threshold = (float)WATER_TEMP_ALERT_THRESHOLD,
    .major_tick_count = 5,  // 60, 80, 100, 120, 140
    .icon_symbol = WATER_SYMBOL,
    .unit_text = "°C",
    .decimal_places = 0
};

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void water_temp_needle_gauge_init(void) {
    // Initialize state
    water_needle_state.is_blinking = false;
    water_needle_state.current_value = water_needle_config.value_min;

    // Create UI components
    water_needle_state.screen_bg = needle_gauge_create_background();
    needle_gauge_create_meter(&water_needle_config, &water_needle_state);
    water_needle_state.value_label = needle_gauge_create_value_label(&water_needle_config);
}

void water_temp_needle_gauge_set_value(int32_t temperature) {
    needle_gauge_update_value(&water_needle_state, &water_needle_config, (float)temperature);
}

#ifdef __cplusplus
}
#endif

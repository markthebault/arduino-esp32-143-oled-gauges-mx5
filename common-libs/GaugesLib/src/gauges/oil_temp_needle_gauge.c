#ifdef __cplusplus
extern "C" {
#endif

#include "oil_temp_needle_gauge.h"
#include "needle_gauge_common.h"

// ============================================================================
// PRIVATE STATE
// ============================================================================

static needle_gauge_state_t oil_needle_state = {0};

static const needle_gauge_config_t oil_needle_config = {
    .value_min = (float)OIL_TEMP_MIN,
    .value_max = (float)OIL_TEMP_MAX,
    .zone_green = (float)OIL_TEMP_ZONE_GREEN,
    .zone_orange = (float)OIL_TEMP_ZONE_ORANGE,
    .zone_red = (float)OIL_TEMP_ZONE_RED,
    .alert_threshold = (float)OIL_TEMP_ALERT_THRESHOLD,
    .major_tick_count = 6,  // 60, 80, 100, 120, 140, 160
    .icon_symbol = OIL_TEMP_SYMBOL,
    .unit_text = "°C",
    .decimal_places = 0
};

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void oil_temp_needle_gauge_init(void) {
    // Initialize state
    oil_needle_state.is_blinking = false;
    oil_needle_state.current_value = oil_needle_config.value_min;

    // Create UI components
    oil_needle_state.screen_bg = needle_gauge_create_background();
    needle_gauge_create_meter(&oil_needle_config, &oil_needle_state);
    oil_needle_state.value_label = needle_gauge_create_value_label(&oil_needle_config);
}

void oil_temp_needle_gauge_set_value(int32_t temperature) {
    needle_gauge_update_value(&oil_needle_state, &oil_needle_config, (float)temperature);
}

#ifdef __cplusplus
}
#endif

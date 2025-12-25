#ifdef __cplusplus
extern "C" {
#endif

#include "oil_pressure_needle_gauge.h"
#include "needle_gauge_common.h"

// ============================================================================
// PRIVATE STATE
// ============================================================================

static needle_gauge_state_t oil_pressure_needle_state = {0};

static const needle_gauge_config_t oil_pressure_needle_config = {
    .value_min = OIL_PRESSURE_MIN,
    .value_max = OIL_PRESSURE_MAX,
    .zone_green = OIL_PRESSURE_ZONE_GREEN,
    .zone_orange = OIL_PRESSURE_ZONE_ORANGE,
    .zone_red = OIL_PRESSURE_ZONE_RED,
    .alert_threshold = OIL_PRESSURE_ALERT_HIGH,
    .major_tick_count = 9,  // 0, 1, 2, 3, 4, 5, 6, 7, 8
    .icon_symbol = OIL_PRESSURE_SYMBOL,
    .unit_text = "bar",
    .decimal_places = 1  // Show one decimal place for pressure
};

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void oil_pressure_needle_gauge_init(void) {
    // Initialize state
    oil_pressure_needle_state.is_blinking = false;
    oil_pressure_needle_state.current_value = oil_pressure_needle_config.value_min;

    // Create UI components
    oil_pressure_needle_state.screen_bg = needle_gauge_create_background();
    needle_gauge_create_meter(&oil_pressure_needle_config, &oil_pressure_needle_state);
    oil_pressure_needle_state.value_label = needle_gauge_create_value_label(&oil_pressure_needle_config);
}

void oil_pressure_needle_gauge_set_value(float pressure, int32_t rpm) {
    // Update the needle gauge
    // Note: For simplicity, we use fixed thresholds here
    // You can add RPM-based dynamic threshold logic if needed
    (void)rpm;  // Unused for now

    needle_gauge_update_value(&oil_pressure_needle_state, &oil_pressure_needle_config, pressure);
}

#ifdef __cplusplus
}
#endif

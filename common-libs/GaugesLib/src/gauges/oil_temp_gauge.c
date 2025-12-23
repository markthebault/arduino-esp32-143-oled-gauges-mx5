#ifdef __cplusplus
extern "C" {
#endif

#include "oil_temp_gauge.h"
#include "gauge_common.h"

// ============================================================================
// PRIVATE STATE
// ============================================================================

static gauge_state_t oil_gauge_state = {0};

static const gauge_config_t oil_gauge_config = {
    .temp_min = OIL_TEMP_MIN,
    .temp_max = OIL_TEMP_MAX,
    .zone_green = OIL_TEMP_ZONE_GREEN,
    .zone_orange = OIL_TEMP_ZONE_ORANGE,
    .zone_red = OIL_TEMP_ZONE_RED,
    .redline = OIL_TEMP_REDLINE,
    .alert_threshold = OIL_TEMP_ALERT_THRESHOLD,
    .marker_interval = OIL_TEMP_MARKER_INTERVAL,
    .icon_symbol = OIL_TEMP_SYMBOL
};

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void oil_temp_gauge_init(void) {
    // Initialize state
    oil_gauge_state.is_blinking = false;

    // Create UI components using common utilities
    oil_gauge_state.screen_bg = gauge_create_background();
    oil_gauge_state.arc = gauge_create_arc(&oil_gauge_config);
    gauge_create_border_and_markers(&oil_gauge_config);
    gauge_create_redline(&oil_gauge_config);
    oil_gauge_state.label = gauge_create_digital_display();
    oil_gauge_state.icon = gauge_create_icon(oil_gauge_config.icon_symbol);
}

void oil_temp_gauge_set_value(int32_t temperature) {
    gauge_update_value(&oil_gauge_state, &oil_gauge_config, temperature);
}

#ifdef __cplusplus
}
#endif

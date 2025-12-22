#ifndef GAUGE_COMMON_H
#define GAUGE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"
#include "gauges_config.h"

// ============================================================================
// GAUGE CONTEXT STRUCTURE
// ============================================================================

/**
 * @brief Configuration context for a gauge instance
 *
 * Contains all parameters needed to create and configure a gauge display.
 * This allows common functions to work with different gauge types.
 */
typedef struct {
    // Temperature range
    int32_t temp_min;
    int32_t temp_max;

    // Color zones
    int32_t zone_green;
    int32_t zone_orange;
    int32_t zone_red;

    // Alert configuration
    int32_t redline;
    int32_t alert_threshold;

    // Display configuration
    int32_t marker_interval;
    const char *icon_symbol;
} gauge_config_t;

/**
 * @brief Runtime state for a gauge instance
 */
typedef struct {
    lv_obj_t *arc;
    lv_obj_t *label;
    lv_obj_t *screen_bg;
    lv_obj_t *icon;
    lv_anim_t blink_anim;
    bool is_blinking;
} gauge_state_t;

// ============================================================================
// ANIMATION CALLBACKS
// ============================================================================

/**
 * @brief Animation callback for arc value changes
 */
void gauge_arc_anim_cb(void *arc, int32_t value);

/**
 * @brief Animation callback for background blink effect
 */
void gauge_bg_blink_anim_cb(void *obj, int32_t value);

// ============================================================================
// ANIMATION CONTROL
// ============================================================================

/**
 * @brief Start the alert blink animation
 * @param state Gauge state containing screen background
 */
void gauge_start_blink(gauge_state_t *state);

/**
 * @brief Stop the alert blink animation
 * @param state Gauge state containing screen background
 */
void gauge_stop_blink(gauge_state_t *state);

// ============================================================================
// UI COMPONENT CREATION
// ============================================================================

/**
 * @brief Create the gauge background screen
 * @return Background object
 */
lv_obj_t* gauge_create_background(void);

/**
 * @brief Create the main temperature arc
 * @param config Gauge configuration
 * @return Arc object
 */
lv_obj_t* gauge_create_arc(const gauge_config_t *config);

/**
 * @brief Create the border and markers around the gauge
 * @param config Gauge configuration
 */
void gauge_create_border_and_markers(const gauge_config_t *config);

/**
 * @brief Create the redline indicator arc
 * @param config Gauge configuration
 */
void gauge_create_redline(const gauge_config_t *config);

/**
 * @brief Create the digital temperature display
 * @return Temperature label object
 */
lv_obj_t* gauge_create_digital_display(void);

/**
 * @brief Create the gauge icon at the bottom
 * @param icon_symbol UTF-8 icon symbol to display
 * @return Icon label object
 */
lv_obj_t* gauge_create_icon(const char *icon_symbol);

// ============================================================================
// GAUGE UPDATE
// ============================================================================

/**
 * @brief Update gauge value with animation and color changes
 * @param state Gauge state
 * @param config Gauge configuration
 * @param temperature New temperature value
 */
void gauge_update_value(gauge_state_t *state, const gauge_config_t *config, int32_t temperature);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Calculate marker gap angle based on temperature range
 * @param temp_min Minimum temperature
 * @param temp_max Maximum temperature
 * @param marker_interval Interval between markers in degrees
 * @return Gap angle in degrees
 */
float gauge_calc_marker_gap(int32_t temp_min, int32_t temp_max, int32_t marker_interval);

#ifdef __cplusplus
}
#endif

#endif // GAUGE_COMMON_H

#ifndef NEEDLE_GAUGE_COMMON_H
#define NEEDLE_GAUGE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"
#include "gauges_config.h"

// ============================================================================
// NEEDLE GAUGE CONTEXT STRUCTURE
// ============================================================================

/**
 * @brief Configuration context for a needle gauge instance using LVGL meter widget
 *
 * This style uses the LVGL meter widget for a clean, professional gauge design.
 */
typedef struct {
    // Value range
    float value_min;
    float value_max;

    // Color zones (for needle color)
    float zone_green;
    float zone_orange;
    float zone_red;

    // Alert configuration
    float alert_threshold;

    // Display configuration
    int32_t major_tick_count;  // Number of major ticks with labels
    const char *icon_symbol;
    const char *unit_text;  // e.g., "°C" or "bar"

    // Value scaling
    int32_t decimal_places;  // Number of decimal places to show in labels
} needle_gauge_config_t;

/**
 * @brief Runtime state for a needle gauge instance
 */
typedef struct {
    lv_obj_t *screen_bg;
    lv_obj_t *meter;
    lv_meter_scale_t *scale;
    lv_meter_indicator_t *needle_indicator;
    lv_obj_t *value_label;
    lv_anim_t blink_anim;
    bool is_blinking;
    float current_value;
} needle_gauge_state_t;

// ============================================================================
// NEEDLE GAUGE DIMENSIONS
// ============================================================================

// Meter size (slightly smaller than screen to add padding)
#define NEEDLE_METER_SIZE        (GAUGE_DIMENSION - (int)(30 * GAUGE_SCALE))
#define NEEDLE_WIDTH             ((int)(5 * GAUGE_SCALE))
#define NEEDLE_CENTER_SIZE       (NEEDLE_METER_SIZE / 10)

// Needle angle range: Start at very bottom (6 o'clock) and sweep 270° clockwise
// In LVGL meter, angles are: 0°=right, 90°=bottom, 180°=left, 270°=top
// We want: min at 90° (bottom), max at 360° (or 0°, which is 270° from 90°)
#define NEEDLE_ANGLE_START  90   // Start angle (6 o'clock, very bottom)
#define NEEDLE_ANGLE_RANGE  270  // 270° sweep clockwise to 9 o'clock (left side)

// ============================================================================
// ANIMATION CALLBACKS
// ============================================================================

/**
 * @brief Animation callback for needle value changes
 */
void needle_gauge_set_value_anim(void *indic, int32_t value);

/**
 * @brief Animation callback for background blink effect
 */
void needle_gauge_bg_blink_cb(void *obj, int32_t value);

// ============================================================================
// ANIMATION CONTROL
// ============================================================================

/**
 * @brief Start the alert blink animation
 */
void needle_gauge_start_blink(needle_gauge_state_t *state);

/**
 * @brief Stop the alert blink animation
 */
void needle_gauge_stop_blink(needle_gauge_state_t *state);

// ============================================================================
// UI COMPONENT CREATION
// ============================================================================

/**
 * @brief Create the needle gauge background
 */
lv_obj_t* needle_gauge_create_background(void);

/**
 * @brief Create the meter gauge with needle
 * @param config Gauge configuration
 * @param state Gauge state (will be populated)
 * @return Meter object
 */
lv_obj_t* needle_gauge_create_meter(const needle_gauge_config_t *config, needle_gauge_state_t *state);

/**
 * @brief Create the value display label in bottom right corner
 * @param config Gauge configuration (for unit text)
 * @return Value label object
 */
lv_obj_t* needle_gauge_create_value_label(const needle_gauge_config_t *config);

// ============================================================================
// NEEDLE GAUGE UPDATE
// ============================================================================

/**
 * @brief Update needle gauge value with animation and color changes
 * @param state Gauge state
 * @param config Gauge configuration
 * @param value New value
 */
void needle_gauge_update_value(needle_gauge_state_t *state, const needle_gauge_config_t *config, float value);

#ifdef __cplusplus
}
#endif

#endif // NEEDLE_GAUGE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "multi_gauge.h"
#include "gauges_config.h"

// ============================================================================
// PRIVATE STATE
// ============================================================================

typedef struct {
    lv_obj_t *bar;
    lv_obj_t *value_label;
    lv_obj_t *unit_label;
    lv_obj_t *icon_label;
    float min_value;
    float max_value;
    float zone_green;
    float zone_orange;
    float zone_red;
} bar_gauge_t;

static bar_gauge_t water_temp_bar;
static bar_gauge_t oil_temp_bar;
static bar_gauge_t oil_pressure_bar;

// ============================================================================
// PRIVATE FUNCTIONS
// ============================================================================

/**
 * @brief Get color based on value and thresholds
 */
static lv_color_t get_color_for_value(float value, float zone_green, float zone_orange, float zone_red) {
    if (value >= zone_red) {
        return COLOR_RED;
    } else if (value >= zone_orange) {
        return COLOR_AMBER;
    } else if (value >= zone_green) {
        return COLOR_GREEN;
    } else {
        return COLOR_GREY;
    }
}

/**
 * @brief Create a horizontal bar gauge row
 */
static void create_bar_gauge_row(bar_gauge_t *gauge, int32_t y_pos, const char *icon_symbol,
                                  const char *text_label,
                                  float min_val, float max_val,
                                  float zone_green, float zone_orange, float zone_red) {
    // Store configuration
    gauge->min_value = min_val;
    gauge->max_value = max_val;
    gauge->zone_green = zone_green;
    gauge->zone_orange = zone_orange;
    gauge->zone_red = zone_red;

    // Create icon/label
    gauge->icon_label = lv_label_create(lv_scr_act());

    #if USE_CUSTOM_ICON_FONT
    lv_label_set_text(gauge->icon_label, icon_symbol);
    lv_obj_set_style_text_font(gauge->icon_label, FONT_ICON, 0);
    #else
    lv_label_set_text(gauge->icon_label, text_label);
    lv_obj_set_style_text_font(gauge->icon_label, FONT_MARKERS, 0);
    #endif

    lv_obj_set_style_text_color(gauge->icon_label, COLOR_WHITE, 0);
    lv_obj_align(gauge->icon_label, LV_ALIGN_LEFT_MID, MULTI_GAUGE_LEFT_PADDING, y_pos);

    // Create horizontal bar
    gauge->bar = lv_bar_create(lv_scr_act());
    lv_obj_set_size(gauge->bar, MULTI_GAUGE_BAR_WIDTH, MULTI_GAUGE_BAR_HEIGHT);
    lv_obj_align(gauge->bar, LV_ALIGN_LEFT_MID, MULTI_GAUGE_LEFT_PADDING + MULTI_GAUGE_ICON_BAR_GAP, y_pos);

    // Style the bar
    lv_obj_set_style_bg_color(gauge->bar, MULTI_GAUGE_BAR_BG_COLOR, LV_PART_MAIN);
    lv_obj_set_style_border_width(gauge->bar, MULTI_GAUGE_BAR_BORDER_WIDTH, LV_PART_MAIN);
    lv_obj_set_style_border_color(gauge->bar, COLOR_GREY, LV_PART_MAIN);
    lv_obj_set_style_radius(gauge->bar, MULTI_GAUGE_BAR_RADIUS, LV_PART_MAIN);

    // Set bar range (cast to int32_t for LVGL)
    lv_bar_set_range(gauge->bar, (int32_t)min_val, (int32_t)max_val);
    lv_bar_set_value(gauge->bar, (int32_t)min_val, LV_ANIM_OFF);

    // Style the indicator (the filled part)
    lv_obj_set_style_bg_color(gauge->bar, COLOR_GREEN, LV_PART_INDICATOR);
    lv_obj_set_style_radius(gauge->bar, MULTI_GAUGE_INDICATOR_RADIUS, LV_PART_INDICATOR);

    // Create value label (numeric value only)
    gauge->value_label = lv_label_create(lv_scr_act());
    lv_label_set_text(gauge->value_label, "---");
    lv_obj_set_style_text_color(gauge->value_label, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(gauge->value_label, FONT_TEMP_UNIT, 0);  // Larger font for number
    lv_obj_align(gauge->value_label, LV_ALIGN_LEFT_MID,
                 MULTI_GAUGE_LEFT_PADDING + MULTI_GAUGE_ICON_BAR_GAP + MULTI_GAUGE_BAR_WIDTH + MULTI_GAUGE_VALUE_X_OFFSET,
                 y_pos);

    // Create unit label (smaller font for units)
    gauge->unit_label = lv_label_create(lv_scr_act());
    lv_label_set_text(gauge->unit_label, "");
    lv_obj_set_style_text_color(gauge->unit_label, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(gauge->unit_label, FONT_MARKERS, 0);  // Smaller font for unit
    // Position will be updated relative to value_label after text is set
}

/**
 * @brief Update a bar gauge with a new value
 */
static void update_bar_gauge(bar_gauge_t *gauge, int32_t value, const char *unit) {
    // Store the actual value for display
    int32_t display_value = value;

    // Constrain bar value to range (for visual indicator only)
    int32_t bar_value = value;
    if (bar_value < (int32_t)gauge->min_value) bar_value = (int32_t)gauge->min_value;
    if (bar_value > (int32_t)gauge->max_value) bar_value = (int32_t)gauge->max_value;

    // Update bar value with animation (constrained to range)
    lv_bar_set_value(gauge->bar, bar_value, LV_ANIM_ON);

    // Update bar color based on constrained value
    lv_color_t color = get_color_for_value(bar_value, gauge->zone_green, gauge->zone_orange, gauge->zone_red);
    lv_obj_set_style_bg_color(gauge->bar, color, LV_PART_INDICATOR);

    // Update value label with actual value (not constrained)
    char buf[16];
    lv_snprintf(buf, sizeof(buf), "%d", (int)display_value);
    lv_label_set_text(gauge->value_label, buf);

    // Update unit label
    lv_label_set_text(gauge->unit_label, unit);

    // Position unit label to the right of value label
    lv_obj_align_to(gauge->unit_label, gauge->value_label, LV_ALIGN_OUT_RIGHT_MID, MULTI_GAUGE_UNIT_SPACING, 0);
}

/**
 * @brief Calculate minimum safe oil pressure based on RPM
 *
 * For a 2.0L naturally aspirated engine (MX5 NC):
 * - Idle (800 RPM): ~0.5 bar minimum
 * - 3000 RPM: ~1.5 bar minimum
 * - 6000 RPM: ~3.0 bar minimum
 *
 * @param rpm Engine RPM
 * @return Minimum safe pressure in bar
 */
static float calculate_min_oil_pressure(int32_t rpm) {
    // Conservative formula: 0.5 bar per 1000 RPM
    float min_pressure = (float)rpm / OIL_PRESSURE_RPM_DIVISOR;

    // Ensure minimum of 0.5 bar at idle
    if (min_pressure < OIL_PRESSURE_MIN_IDLE) {
        min_pressure = OIL_PRESSURE_MIN_IDLE;
    }

    return min_pressure;
}

/**
 * @brief Update oil pressure bar gauge (handles float values)
 */
static void update_pressure_bar_gauge(bar_gauge_t *gauge, float value, int32_t rpm) {
    // Constrain value to range
    if (value < gauge->min_value) value = gauge->min_value;
    if (value > gauge->max_value) value = gauge->max_value;

    // Update bar value with animation (convert float to int for bar)
    int32_t bar_value = (int32_t)(value * OIL_PRESSURE_RESOLUTION_MULT); // Scale for better resolution
    int32_t bar_max = (int32_t)(gauge->max_value * OIL_PRESSURE_RESOLUTION_MULT);
    lv_bar_set_range(gauge->bar, 0, bar_max);
    lv_bar_set_value(gauge->bar, bar_value, LV_ANIM_ON);

    // Calculate dynamic minimum pressure threshold based on RPM
    float min_safe_pressure = calculate_min_oil_pressure(rpm);

    // Update bar color based on value with RPM-dependent low threshold
    // Oil pressure is critical both when too low (relative to RPM) and too high
    lv_color_t color;
    if (value < min_safe_pressure) {
        color = COLOR_RED;   // Too low for current RPM - dangerous!
    } else if (value >= gauge->zone_red) {
        color = COLOR_RED;   // Too high - dangerous!
    } else if (value >= gauge->zone_orange) {
        color = COLOR_AMBER; // Getting high - warning
    } else {
        color = COLOR_GREEN; // Good range
    }
    lv_obj_set_style_bg_color(gauge->bar, color, LV_PART_INDICATOR);

    // Update value label with one decimal place (number only)
    char buf[16];
    lv_snprintf(buf, sizeof(buf), "%d.%d", (int)value, (int)((value - (int)value) * OIL_PRESSURE_DECIMAL_MULT));
    lv_label_set_text(gauge->value_label, buf);

    // Update unit label
    lv_label_set_text(gauge->unit_label, "bar");

    // Position unit label to the right of value label
    lv_obj_align_to(gauge->unit_label, gauge->value_label, LV_ALIGN_OUT_RIGHT_MID, MULTI_GAUGE_UNIT_SPACING, 0);
}

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void multi_gauge_init(void) {
    // Calculate vertical positions for three rows, centered on screen
    int32_t screen_center_y = 0;
    int32_t row1_y = screen_center_y - MULTI_GAUGE_ROW_SPACING;
    int32_t row2_y = screen_center_y;
    int32_t row3_y = screen_center_y + MULTI_GAUGE_ROW_SPACING;

    // Create oil pressure row (top)
    create_bar_gauge_row(
        &oil_pressure_bar, row1_y, OIL_PRESSURE_SYMBOL, OIL_PRES_TEXT_LABEL,
        OIL_PRESSURE_MIN, OIL_PRESSURE_MAX,
        OIL_PRESSURE_ZONE_GREEN, OIL_PRESSURE_ZONE_ORANGE, OIL_PRESSURE_ZONE_RED
    );

    // Create oil temperature row (middle)
    create_bar_gauge_row(
        &oil_temp_bar, row2_y, OIL_TEMP_SYMBOL, OIL_TEMP_TEXT_LABEL,
        OIL_TEMP_MIN, OIL_TEMP_MAX,
        OIL_TEMP_ZONE_GREEN, OIL_TEMP_ZONE_ORANGE, OIL_TEMP_ZONE_RED
    );

    // Create water temperature row (bottom)
    create_bar_gauge_row(
        &water_temp_bar, row3_y, WATER_SYMBOL, WATER_TEXT_LABEL,
        WATER_TEMP_MIN, WATER_TEMP_MAX,
        WATER_TEMP_ZONE_GREEN, WATER_TEMP_ZONE_ORANGE, WATER_TEMP_ZONE_RED
    );
}

void multi_gauge_set_values(int32_t water_temp, int32_t oil_temp, float oil_pressure, int32_t rpm) {
    update_bar_gauge(&water_temp_bar, water_temp, "°C");
    update_bar_gauge(&oil_temp_bar, oil_temp, "°C");
    update_pressure_bar_gauge(&oil_pressure_bar, oil_pressure, rpm);
}

#ifdef __cplusplus
}
#endif

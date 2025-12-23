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
    lv_obj_t *icon_label;
    int32_t min_value;
    int32_t max_value;
    int32_t zone_green;
    int32_t zone_orange;
    int32_t zone_red;
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
static lv_color_t get_color_for_value(int32_t value, int32_t zone_green, int32_t zone_orange, int32_t zone_red) {
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
                                  int32_t min_val, int32_t max_val,
                                  int32_t zone_green, int32_t zone_orange, int32_t zone_red) {
    // Store configuration
    gauge->min_value = min_val;
    gauge->max_value = max_val;
    gauge->zone_green = zone_green;
    gauge->zone_orange = zone_orange;
    gauge->zone_red = zone_red;

    // Create icon label
    gauge->icon_label = lv_label_create(lv_scr_act());
    lv_label_set_text(gauge->icon_label, icon_symbol);
    lv_obj_set_style_text_color(gauge->icon_label, COLOR_WHITE, 0);

    #if USE_CUSTOM_ICON_FONT
    lv_obj_set_style_text_font(gauge->icon_label, FONT_ICON, 0);
    #else
    lv_obj_set_style_text_font(gauge->icon_label, FONT_MARKERS, 0);
    #endif

    lv_obj_align(gauge->icon_label, LV_ALIGN_LEFT_MID, (int)(20 * GAUGE_SCALE), y_pos);

    // Create horizontal bar
    gauge->bar = lv_bar_create(lv_scr_act());
    lv_obj_set_size(gauge->bar, MULTI_GAUGE_BAR_WIDTH, MULTI_GAUGE_BAR_HEIGHT);
    lv_obj_align(gauge->bar, LV_ALIGN_LEFT_MID, (int)(80 * GAUGE_SCALE), y_pos);

    // Style the bar
    lv_obj_set_style_bg_color(gauge->bar, lv_color_hex(0x1A1A1A), LV_PART_MAIN);
    lv_obj_set_style_border_width(gauge->bar, (int)(2 * GAUGE_SCALE), LV_PART_MAIN);
    lv_obj_set_style_border_color(gauge->bar, COLOR_GREY, LV_PART_MAIN);
    lv_obj_set_style_radius(gauge->bar, (int)(4 * GAUGE_SCALE), LV_PART_MAIN);

    // Set bar range
    lv_bar_set_range(gauge->bar, min_val, max_val);
    lv_bar_set_value(gauge->bar, min_val, LV_ANIM_OFF);

    // Style the indicator (the filled part)
    lv_obj_set_style_bg_color(gauge->bar, COLOR_GREEN, LV_PART_INDICATOR);
    lv_obj_set_style_radius(gauge->bar, (int)(2 * GAUGE_SCALE), LV_PART_INDICATOR);

    // Create value label
    gauge->value_label = lv_label_create(lv_scr_act());
    lv_label_set_text(gauge->value_label, "---");
    lv_obj_set_style_text_color(gauge->value_label, COLOR_WHITE, 0);
    lv_obj_set_style_text_font(gauge->value_label, FONT_TEMP_UNIT, 0);
    lv_obj_align(gauge->value_label, LV_ALIGN_LEFT_MID,
                 (int)(80 * GAUGE_SCALE) + MULTI_GAUGE_BAR_WIDTH + (int)(20 * GAUGE_SCALE),
                 y_pos);
}

/**
 * @brief Update a bar gauge with a new value
 */
static void update_bar_gauge(bar_gauge_t *gauge, int32_t value, const char *unit) {
    // Constrain value to range
    if (value < gauge->min_value) value = gauge->min_value;
    if (value > gauge->max_value) value = gauge->max_value;

    // Update bar value with animation
    lv_bar_set_value(gauge->bar, value, LV_ANIM_ON);

    // Update bar color based on value
    lv_color_t color = get_color_for_value(value, gauge->zone_green, gauge->zone_orange, gauge->zone_red);
    lv_obj_set_style_bg_color(gauge->bar, color, LV_PART_INDICATOR);

    // Update value label
    char buf[32];
    lv_snprintf(buf, sizeof(buf), "%d %s", (int)value, unit);
    lv_label_set_text(gauge->value_label, buf);
}

/**
 * @brief Update oil pressure bar gauge (handles float values)
 */
static void update_pressure_bar_gauge(bar_gauge_t *gauge, float value) {
    // Constrain value to range
    if (value < gauge->min_value) value = gauge->min_value;
    if (value > gauge->max_value) value = gauge->max_value;

    // Update bar value with animation (convert float to int for bar)
    int32_t bar_value = (int32_t)(value * 10); // Scale for better resolution
    int32_t bar_max = gauge->max_value * 10;
    lv_bar_set_range(gauge->bar, 0, bar_max);
    lv_bar_set_value(gauge->bar, bar_value, LV_ANIM_ON);

    // Update bar color based on value
    lv_color_t color;
    if (value < gauge->zone_green) {
        color = COLOR_GREY;  // Too low
    } else if (value >= gauge->zone_red) {
        color = COLOR_RED;   // Too high
    } else if (value >= gauge->zone_orange) {
        color = COLOR_AMBER; // Getting high
    } else {
        color = COLOR_GREEN; // Good range
    }
    lv_obj_set_style_bg_color(gauge->bar, color, LV_PART_INDICATOR);

    // Update value label with one decimal place
    char buf[32];
    lv_snprintf(buf, sizeof(buf), "%d.%d bar", (int)value, (int)((value - (int)value) * 10));
    lv_label_set_text(gauge->value_label, buf);
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

    // Create oil pressure row
    create_bar_gauge_row(
        &oil_pressure_bar, row3_y, OIL_SYMBOL,
        OIL_PRESSURE_MIN, OIL_PRESSURE_MAX,
        OIL_PRESSURE_ZONE_GREEN, OIL_PRESSURE_ZONE_ORANGE, OIL_PRESSURE_ZONE_RED
    );

    // Create oil temperature row
    create_bar_gauge_row(
        &oil_temp_bar, row2_y, OIL_SYMBOL,
        OIL_TEMP_MIN, OIL_TEMP_MAX,
        OIL_TEMP_ZONE_GREEN, OIL_TEMP_ZONE_ORANGE, OIL_TEMP_ZONE_RED
    );
    
                         // Create water temperature row
    create_bar_gauge_row(
        &water_temp_bar, row1_y, WATER_SYMBOL,
        WATER_TEMP_MIN, WATER_TEMP_MAX,
        WATER_TEMP_ZONE_GREEN, WATER_TEMP_ZONE_ORANGE, WATER_TEMP_ZONE_RED
    );
}

void multi_gauge_set_values(int32_t water_temp, int32_t oil_temp, float oil_pressure) {
    update_bar_gauge(&water_temp_bar, water_temp, "°C");
    update_bar_gauge(&oil_temp_bar, oil_temp, "°C");
    update_pressure_bar_gauge(&oil_pressure_bar, oil_pressure);
}

#ifdef __cplusplus
}
#endif

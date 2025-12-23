#ifndef GAUGES_CONFIG_H
#define GAUGES_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

// ============================================================================
// SCREEN PROFILE SELECTION - PICK ONE
// ============================================================================

#if !defined(USE_SCREEN_466PX) && !defined(USE_SCREEN_240PX)
    #define USE_SCREEN_466PX 
    #warning "No screen profile defined in build flags. Defaulting to 466px."
#endif

// ============================================================================
// SCREEN PROFILE DEFINITIONS
// ============================================================================

#ifdef USE_SCREEN_466PX
    // 466px display - Original configuration (baseline, no scaling)
    #define LCD_RES 466
    #define GAUGE_SCALE 1.0f
    // Fonts for 466px display (using custom fonts)
    // Custom font declarations (defined in gauge_common.c)
    extern const lv_font_t montserrat_bold_number_120;
    extern const lv_font_t fa_icons_54;
    #define FONT_TEMP_MAIN   &montserrat_bold_number_120  // Main temperature number (120px)
    #define FONT_TEMP_UNIT   &lv_font_montserrat_48       // °C unit (48px)
    #define FONT_MARKERS     &lv_font_montserrat_24       // Gauge markers (24px)
    #define FONT_ICON        &fa_icons_54                 // Bottom icon (54px)
    #define USE_CUSTOM_TEMP_FONT 1
    #define USE_CUSTOM_ICON_FONT 1
#endif

#ifdef USE_SCREEN_240PX
    // 240px display - Scaled down from 466px baseline
    #define LCD_RES 240
    #define GAUGE_SCALE 0.515f  // 240/466 ≈ 0.515
    // Fonts for 240px display (using LVGL built-in fonts)
    // Note: For optimal display, consider generating custom fonts at scaled sizes
    #define FONT_TEMP_MAIN   &lv_font_montserrat_48     // Main temperature number (scaled from 120px)
    #define FONT_TEMP_UNIT   &lv_font_montserrat_24     // °C unit (scaled from 48px)
    #define FONT_MARKERS     &lv_font_montserrat_14     // Gauge markers (scaled from 24px)
    #define FONT_ICON        &lv_font_montserrat_32     // Bottom icon (scaled from 54px)
    #define USE_CUSTOM_TEMP_FONT 0
    #define USE_CUSTOM_ICON_FONT 0
#endif

// ============================================================================
// COLOR PALETTE
// ============================================================================

#define COLOR_BLACK   lv_color_hex(0x000000)
#define COLOR_WHITE   lv_color_hex(0xE6E6E6)   // Not full white. Reduces glare and burn-in
#define COLOR_GREY    lv_color_hex(0x7A7A7A)   // Neutral UI text
#define COLOR_GREEN   lv_color_hex(0x00C853)   // Primary normal state
#define COLOR_AMBER   lv_color_hex(0xF2A900)   // Warning. Slightly less harsh than FA8C00
#define COLOR_RED     lv_color_hex(0xE53935)   // Alert. OLED-safe red

// ============================================================================
// ICON SYMBOLS AND TEXT LABELS
// ============================================================================

// Icon symbols (used when USE_CUSTOM_ICON_FONT is enabled)
#define FUEL_SYMBOL   "\xEF\x94\xAF"  // U+F52F
#define OIL_SYMBOL    "\xEF\x98\x93"  // U+F613 - Droplet
#define WATER_SYMBOL  "\xEF\x8B\x89"  // U+F2C9 - Thermometer

// Text labels (used when USE_CUSTOM_ICON_FONT is disabled)
#define WATER_TEXT_LABEL    "H2O"
#define OIL_TEMP_TEXT_LABEL "OIL T"
#define OIL_PRES_TEXT_LABEL "OIL P"

// ============================================================================
// COMMON GAUGE DIMENSIONS (Scaled)
// ============================================================================

// Calculate the gauge dimension based on screen resolution
#define GAUGE_DIMENSION LCD_RES

// Arc geometry (angles are independent of scale)
#define ARC_START_ANGLE  135
#define ARC_END_ANGLE    405  // 135 + 270 degrees

// Visual styling (scaled based on GAUGE_SCALE)
#define ARC_WIDTH        ((int)(24 * GAUGE_SCALE))
#define LINE_WIDTH       ((int)(4 * GAUGE_SCALE))
#define ARC_SIZE         (GAUGE_DIMENSION - (LINE_WIDTH * 12))

// Positioning offsets (scaled based on GAUGE_SCALE)
#define MARKER_LABEL_OFFSET     ((int)(40 * GAUGE_SCALE))   // Distance from arc to marker labels
#define TEMP_LABEL_Y_OFFSET     ((int)(-15 * GAUGE_SCALE))  // Y offset for main temperature label
#define TEMP_UNIT_Y_OFFSET      ((int)(55 * GAUGE_SCALE))   // Y offset for unit label
#define ICON_Y_OFFSET           ((int)(-10 * GAUGE_SCALE))  // Y offset for icon from bottom

// ============================================================================
// OIL TEMPERATURE GAUGE CONFIGURATION
// ============================================================================

#define OIL_TEMP_MIN              60
#define OIL_TEMP_MAX              160
#define OIL_TEMP_ZONE_GREEN       80
#define OIL_TEMP_ZONE_ORANGE      120
#define OIL_TEMP_ZONE_RED         130
#define OIL_TEMP_REDLINE          130
#define OIL_TEMP_ALERT_THRESHOLD  135
#define OIL_TEMP_MARKER_INTERVAL  20   // Markers every 20 degrees

// ============================================================================
// WATER TEMPERATURE GAUGE CONFIGURATION
// ============================================================================

#define WATER_TEMP_MIN              60
#define WATER_TEMP_MAX              140
#define WATER_TEMP_ZONE_GREEN       80
#define WATER_TEMP_ZONE_ORANGE      105
#define WATER_TEMP_ZONE_RED         110
#define WATER_TEMP_REDLINE          110
#define WATER_TEMP_ALERT_THRESHOLD  115
#define WATER_TEMP_MARKER_INTERVAL  20   // Markers every 20 degrees

// ============================================================================
// OIL PRESSURE GAUGE CONFIGURATION
// ============================================================================

#define OIL_PRESSURE_MIN              0.0f
#define OIL_PRESSURE_MAX              8.0f
#define OIL_PRESSURE_ZONE_GREEN       2.0f
#define OIL_PRESSURE_ZONE_ORANGE      5.0f
#define OIL_PRESSURE_ZONE_RED         7.0f
#define OIL_PRESSURE_REDLINE          8.0f
#define OIL_PRESSURE_ALERT_LOW        1.0f    // Alert when pressure too low
#define OIL_PRESSURE_ALERT_HIGH       7.0f    // Alert when pressure too high

// ============================================================================
// MULTI GAUGE BAR CONFIGURATION
// ============================================================================

#define MULTI_GAUGE_LEFT_PADDING    ((int)(30 * GAUGE_SCALE))  // Left edge padding
#define MULTI_GAUGE_BAR_WIDTH       ((int)(200 * GAUGE_SCALE))
#define MULTI_GAUGE_BAR_HEIGHT      ((int)(40 * GAUGE_SCALE))
#define MULTI_GAUGE_ROW_SPACING     ((int)(85 * GAUGE_SCALE))
#define MULTI_GAUGE_ICON_SIZE       ((int)(48 * GAUGE_SCALE))
#define MULTI_GAUGE_VALUE_WIDTH     ((int)(110 * GAUGE_SCALE))
#define MULTI_GAUGE_ICON_BAR_GAP    ((int)(80 * GAUGE_SCALE))  // Gap between icon and bar start

// ============================================================================
// DEFAULT GAUGE SELECTION
// ============================================================================

// Define which gauge to show on startup
// 0 = Oil Temperature Gauge
// 1 = Water Temperature Gauge
// 2 = Multi Gauge (shows all three: water temp, oil temp, oil pressure)
// 3 = Oil Pressure Gauge
#ifndef DEFAULT_GAUGE
    #define DEFAULT_GAUGE 0  // Default to oil temp gauge
#endif

// ============================================================================
// ANIMATION CONFIGURATION
// ============================================================================

#define GAUGE_ANIM_TIME       200   // ms - Gauge needle animation
#define BLINK_ANIM_TIME       250   // ms - Alert blink animation
#define BLINK_PLAYBACK_TIME   250   // ms - Alert blink reverse animation

#ifdef __cplusplus
}
#endif

#endif // GAUGES_CONFIG_H

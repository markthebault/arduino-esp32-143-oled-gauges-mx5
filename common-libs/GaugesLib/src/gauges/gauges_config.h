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
    extern const lv_font_t montserrat_bold_numbers_120;
    extern const lv_font_t fa_icons_54;
    #define FONT_TEMP_MAIN   &montserrat_bold_numbers_120  // Main temperature number (120px)
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
    extern const lv_font_t fa_icons_32;
    #define FONT_TEMP_MAIN   &lv_font_montserrat_48     // Main temperature number (scaled from 120px)
    #define FONT_TEMP_UNIT   &lv_font_montserrat_24     // °C unit (scaled from 48px)
    #define FONT_MARKERS     &lv_font_montserrat_14     // Gauge markers (scaled from 24px)
    #define FONT_ICON        &fa_icons_32            // Bottom icon (scaled from 54px)
    #define USE_CUSTOM_TEMP_FONT 0
    #define USE_CUSTOM_ICON_FONT 1
#endif

// ============================================================================
// COLOR PALETTE
// ============================================================================

// Base colors
#define COLOR_BLACK   lv_color_hex(0x000000)
#define COLOR_WHITE   lv_color_hex(0xE6E6E6)   // Not full white. Reduces glare and burn-in
#define COLOR_GREY    lv_color_hex(0x7A7A7A)   // Neutral UI text
#define COLOR_GREEN   lv_color_hex(0x00C853)   // Primary normal state
#define COLOR_AMBER   lv_color_hex(0xF2A900)   // Warning. Slightly less harsh than FA8C00
#define COLOR_RED     lv_color_hex(0xE53935)   // Alert. OLED-safe red

// Screen and background colors
#define COLOR_SCREEN_BG         COLOR_BLACK     // Main screen background

// Gauge component colors
#define COLOR_ARC_BORDER        COLOR_WHITE     // Arc gauge border
#define COLOR_ARC_BACKGROUND    COLOR_BLACK     // Arc gauge background
#define COLOR_MARKER            COLOR_WHITE     // Arc gauge tick markers
#define COLOR_MARKER_LABEL      COLOR_WHITE     // Arc gauge marker labels
#define COLOR_REDLINE           COLOR_RED       // Redline indicator

// Text and label colors
#define COLOR_VALUE_TEXT        COLOR_WHITE     // Main value display
#define COLOR_UNIT_TEXT         COLOR_AMBER     // Unit labels (°C, bar, etc.)
#define COLOR_ICON              COLOR_AMBER     // Icon symbols

// Needle gauge colors
#define COLOR_NEEDLE_MINOR_TICK lv_color_hex(0x666666)  // Minor tick marks (gray)
#define COLOR_NEEDLE_MAJOR_TICK COLOR_WHITE             // Major tick marks
#define COLOR_NEEDLE_LABEL      COLOR_WHITE             // Needle gauge scale labels

// ============================================================================
// ICON SYMBOLS AND TEXT LABELS
// ============================================================================

// Icon symbols (used when USE_CUSTOM_ICON_FONT is enabled)
#define FUEL_SYMBOL         "\xEF\x94\xAF"  // U+F52F
#define OIL_TEMP_SYMBOL     "\xEF\x98\x93"  // U+F613 - Oil Can (droplet)
#define OIL_PRESSURE_SYMBOL "\xEF\x98\x94"  // U+F614 - Oil pressure
#define WATER_SYMBOL        "\xEF\x8B\x89"  // U+F2C9 - Thermometer

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

#define WATER_TEMP_MIN              50
#define WATER_TEMP_MAX              130
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
// NEEDLE GAUGE CONFIGURATION
// ============================================================================

// Meter size (slightly smaller than screen to add padding)
#define NEEDLE_METER_SIZE        (GAUGE_DIMENSION - (int)(30 * GAUGE_SCALE))
#define NEEDLE_WIDTH             ((int)(5 * GAUGE_SCALE))
#define NEEDLE_CENTER_SIZE       (NEEDLE_METER_SIZE / 10)

// Needle angle range: Start at very bottom (6 o'clock) and sweep 270° clockwise
// In LVGL meter, angles are: 0°=right, 90°=bottom, 180°=left, 270°=top
// We want: min at 90° (bottom), max at 360° (or 0°, which is 270° from 90°)
#define NEEDLE_ANGLE_START  135   // Start angle (6 o'clock, very bottom)
#define NEEDLE_ANGLE_RANGE  270  // 270° sweep clockwise to 9 o'clock (left side)

// Minor tick calculation
#define NEEDLE_MINOR_TICK_MULTIPLIER    2       // Multiplier for pressure gauges (decimal places)
#define NEEDLE_MINOR_TICK_DIVISOR       10      // Divisor for temperature gauges (integer)

// Tick styling (scaled)
#define NEEDLE_MINOR_TICK_LENGTH        ((int)(8 * GAUGE_SCALE))
#define NEEDLE_MINOR_TICK_WIDTH         1
#define NEEDLE_MINOR_TICK_COLOR         COLOR_NEEDLE_MINOR_TICK  // Minor tick color

#define NEEDLE_MAJOR_TICK_WIDTH         3
#define NEEDLE_MAJOR_TICK_LENGTH        ((int)(18 * GAUGE_SCALE))
#define NEEDLE_MAJOR_TICK_LABEL_DIST    ((int)(29 * GAUGE_SCALE))

// Red zone arc styling (scaled)
#define NEEDLE_RED_ZONE_WIDTH           ((int)(18 * GAUGE_SCALE))
#define NEEDLE_RED_ZONE_OFFSET          ((int)(0 * GAUGE_SCALE))

// Needle styling (scaled)
#define NEEDLE_OFFSET                   ((int)(-20 * GAUGE_SCALE))

// Center knob styling (scaled)
#define NEEDLE_CENTER_BG_COLOR          lv_color_hex(0x151515)
#define NEEDLE_CENTER_BORDER_COLOR      lv_color_hex(0x444444)
#define NEEDLE_CENTER_BORDER_WIDTH      2

// Label positioning (scaled)
#define NEEDLE_VALUE_LABEL_X_OFFSET     ((int)(15/2 * GAUGE_SCALE))
#define NEEDLE_VALUE_LABEL_Y_OFFSET     ((int)(160 * GAUGE_SCALE))
#define NEEDLE_UNIT_LABEL_X_OFFSET      ((int)(140 * GAUGE_SCALE))
#define NEEDLE_UNIT_LABEL_Y_OFFSET      ((int)(100 * GAUGE_SCALE))
#define NEEDLE_ICON_PADDING_DIVISOR     2       // Used for: (int)15 / NEEDLE_ICON_PADDING_DIVISOR
#define NEEDLE_ICON_Y_OFFSET            ((int)(-80 * GAUGE_SCALE))

// ============================================================================
// ARC GAUGE CONFIGURATION
// ============================================================================

// Marker sizing multipliers
#define ARC_MARKER_SIZE_MULTIPLIER      4       // Used for: LINE_WIDTH * 4
#define ARC_ANGLE_OFFSET_START          2       // Arc start angle adjustment
#define ARC_ANGLE_OFFSET_END            1       // Arc end angle adjustment
#define ARC_MARKER_ANGLE_OFFSET         1       // Marker angle offset

// Border sizing multipliers
#define ARC_BORDER_WIDTH_MULTIPLIER     2       // For: ARC_WIDTH * 2
#define ARC_BORDER_LINE_MULTIPLIER      2       // For: LINE_WIDTH * 2

// Math constants
#define MATH_PI                         3.14159f
#define DEGREES_TO_RADIANS              180.0f

// Redline styling
#define ARC_REDLINE_WIDTH_MULTIPLIER    2       // For: LINE_WIDTH * 2

// ============================================================================
// MULTI GAUGE BAR CONFIGURATION
// ============================================================================

#define MULTI_GAUGE_LEFT_PADDING        ((int)(30 * GAUGE_SCALE))  // Left edge padding
#define MULTI_GAUGE_BAR_WIDTH           ((int)(185 * GAUGE_SCALE))
#define MULTI_GAUGE_BAR_HEIGHT          ((int)(40 * GAUGE_SCALE))
#define MULTI_GAUGE_ROW_SPACING         ((int)(85 * GAUGE_SCALE))
#define MULTI_GAUGE_ICON_SIZE           ((int)(48 * GAUGE_SCALE))
#define MULTI_GAUGE_VALUE_WIDTH         ((int)(110 * GAUGE_SCALE))
#define MULTI_GAUGE_ICON_BAR_GAP        ((int)(100 * GAUGE_SCALE))  // Gap between icon and bar start

// Bar styling
#define MULTI_GAUGE_BAR_BG_COLOR        lv_color_hex(0x1A1A1A)
#define MULTI_GAUGE_BAR_BORDER_WIDTH    ((int)(2 * GAUGE_SCALE))
#define MULTI_GAUGE_BAR_RADIUS          ((int)(4 * GAUGE_SCALE))
#define MULTI_GAUGE_INDICATOR_RADIUS    ((int)(2 * GAUGE_SCALE))

// Label spacing
#define MULTI_GAUGE_VALUE_X_OFFSET      ((int)(25 * GAUGE_SCALE))
#define MULTI_GAUGE_UNIT_SPACING        ((int)(3 * GAUGE_SCALE))

// Oil pressure calculation constants
#define OIL_PRESSURE_RPM_DIVISOR        2000.0f  // Pressure = RPM / 2000
#define OIL_PRESSURE_MIN_IDLE           0.5f     // Minimum pressure at idle (bar)
#define OIL_PRESSURE_RESOLUTION_MULT    10.0f    // Multiplier for bar resolution
#define OIL_PRESSURE_DECIMAL_MULT       10       // For decimal place calculation

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

// ============================================================================
// GESTURE CONFIGURATION
// ============================================================================

// Gesture support for gauge switching:
// - LV_DIR_RIGHT (swipe right): Switch to next gauge
// - LV_DIR_LEFT (swipe left): Switch to previous gauge
// Both directions are automatically enabled when using gauge_manager_enable_gestures()
#define GAUGE_SWITCH_GESTURE_DIR  LV_DIR_RIGHT  // Kept for backwards compatibility

#ifdef __cplusplus
}
#endif

#endif // GAUGES_CONFIG_H

#ifndef GAUGES_CONFIG_H
#define GAUGES_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "../display/lcd_config.h"

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
// ICON SYMBOLS
// ============================================================================

#define FUEL_SYMBOL   "\xEF\x94\xAF"  // U+F52F
#define OIL_SYMBOL    "\xEF\x98\x93"  // U+F613 - Droplet
#define WATER_SYMBOL  "\xEF\x8B\x89"  // U+F2C9 - Thermometer

// ============================================================================
// COMMON GAUGE DIMENSIONS
// ============================================================================

// Calculate the larger dimension for square gauges
#define GAUGE_DIMENSION ((EXAMPLE_LCD_H_RES > EXAMPLE_LCD_V_RES) ? EXAMPLE_LCD_H_RES : EXAMPLE_LCD_V_RES)

// Arc geometry
#define ARC_START_ANGLE  135
#define ARC_END_ANGLE    405  // 135 + 270 degrees

// Visual styling
#define ARC_WIDTH        24
#define LINE_WIDTH       4
#define ARC_SIZE         (GAUGE_DIMENSION - (LINE_WIDTH * 12))

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
#define WATER_TEMP_MARKER_INTERVAL  10   // Markers every 10 degrees

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

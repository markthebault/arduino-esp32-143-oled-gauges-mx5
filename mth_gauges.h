#ifndef MTH_GAUGES_H
#define MTH_GAUGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"
#include "lcd_config.h"

/**
 * Initialize and display the dark speedometer gauge.
 */
void mth_gauge_speedometer_dark(void);

/**
 * Set the speed value of the dark speedometer gauge (0..240).
 * Call this from loop() to update the gauge value directly (replaces animation).
 */
void mth_gauge_set_speed(int32_t v);

#ifdef __cplusplus
}
#endif

#endif // MTH_GAUGES_H
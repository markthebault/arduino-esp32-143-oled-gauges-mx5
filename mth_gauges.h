#ifndef MTH_GAUGES_H
#define MTH_GAUGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"
#include "lcd_config.h"

// Original Speedometer (if still needed)
void mth_gauge_speedometer_dark(void);
void mth_gauge_set_speed(int32_t v);

// New Oil Gauges
/**
 * Initialization functions for the two gauges
 */
void mth_gauge_oil_pressure_init(void);
void mth_gauge_oil_temp_init(void);

/**
 * Setter functions to update the needle positions
 */
void mth_gauge_set_pressure(float v);
void mth_gauge_set_temp(int32_t v);

#ifdef __cplusplus
}
#endif

#endif // MTH_GAUGES_H
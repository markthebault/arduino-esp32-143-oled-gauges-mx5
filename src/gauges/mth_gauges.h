#ifndef MTH_GAUGES_H
#define MTH_GAUGES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"
#include "../display/lcd_config.h"

/**
 * Initialize the oil temperature gauge
 */
void mth_gauge_oil_temp_init(void);

/**
 * Set the temperature value (0..180 Celsius)
 */
void mth_gauge_set_temp(int32_t v);

#ifdef __cplusplus
}
#endif

#endif // MTH_GAUGES_H
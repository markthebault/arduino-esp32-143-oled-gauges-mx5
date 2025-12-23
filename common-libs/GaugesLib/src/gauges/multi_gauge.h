#ifndef MULTI_GAUGE_H
#define MULTI_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

/**
 * @brief Initialize the multi-gauge display
 *
 * Creates a screen showing three horizontal bar gauges:
 * - Water temperature
 * - Oil temperature
 * - Oil pressure
 *
 * Each gauge displays an icon, colored horizontal bar, and numeric value.
 * Should be called once during setup after LVGL is initialized.
 */
void multi_gauge_init(void);

/**
 * @brief Update all values in the multi-gauge display
 *
 * @param water_temp Water temperature in °C
 * @param oil_temp Oil temperature in °C
 * @param oil_pressure Oil pressure in bar
 *
 * Updates all three gauges with new values, including bar colors
 * and value displays.
 */
void multi_gauge_set_values(int32_t water_temp, int32_t oil_temp, float oil_pressure);

#ifdef __cplusplus
}
#endif

#endif // MULTI_GAUGE_H

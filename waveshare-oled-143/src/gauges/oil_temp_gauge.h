#ifndef OIL_TEMP_GAUGE_H
#define OIL_TEMP_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

/**
 * @brief Initialize the oil temperature gauge display
 *
 * Creates all UI components for the oil temperature gauge including:
 * - Background screen
 * - Temperature arc with color zones
 * - Border and markers
 * - Redline indicator
 * - Digital temperature display
 * - Oil icon
 */
void oil_temp_gauge_init(void);

/**
 * @brief Update the oil temperature gauge value
 *
 * @param temperature Temperature value in Celsius (60-160°C)
 *
 * Updates the gauge display with the new temperature value, including:
 * - Arc indicator position (animated)
 * - Color based on temperature zones (grey/green/orange/red)
 * - Digital temperature readout
 * - Alert animation if temperature exceeds threshold
 */
void oil_temp_gauge_set_value(int32_t temperature);

#ifdef __cplusplus
}
#endif

#endif // OIL_TEMP_GAUGE_H

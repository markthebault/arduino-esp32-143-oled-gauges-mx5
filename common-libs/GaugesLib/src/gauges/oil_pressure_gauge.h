#ifndef OIL_PRESSURE_GAUGE_H
#define OIL_PRESSURE_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

/**
 * @brief Initialize the oil pressure gauge display
 *
 * Creates all UI components for the oil pressure gauge including:
 * - Background screen
 * - Pressure arc with color zones
 * - Border and markers
 * - Redline indicator
 * - Digital pressure display
 * - Oil icon
 */
void oil_pressure_gauge_init(void);

/**
 * @brief Update the oil pressure gauge value
 *
 * @param pressure Pressure value in bar (0-8 bar)
 * @param rpm Engine RPM (used for dynamic minimum threshold)
 *
 * Updates the gauge display with the new pressure value, including:
 * - Arc indicator position (animated)
 * - Color based on pressure zones and RPM (grey/green/orange/red)
 * - Digital pressure readout
 * - Alert animation if pressure is too low or too high
 */
void oil_pressure_gauge_set_value(float pressure, int32_t rpm);

#ifdef __cplusplus
}
#endif

#endif // OIL_PRESSURE_GAUGE_H

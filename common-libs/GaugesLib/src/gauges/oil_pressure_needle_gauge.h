#ifndef OIL_PRESSURE_NEEDLE_GAUGE_H
#define OIL_PRESSURE_NEEDLE_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initialize oil pressure needle gauge (normal mode)
 *
 * Creates a simple needle gauge with tick markers and value display
 * in the bottom right corner.
 */
void oil_pressure_needle_gauge_init(void);

/**
 * @brief Set the oil pressure value
 *
 * @param pressure Oil pressure in bar
 * @param rpm Engine RPM (used for dynamic pressure thresholds)
 */
void oil_pressure_needle_gauge_set_value(float pressure, int32_t rpm);

#ifdef __cplusplus
}
#endif

#endif // OIL_PRESSURE_NEEDLE_GAUGE_H

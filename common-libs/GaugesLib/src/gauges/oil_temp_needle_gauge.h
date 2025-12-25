#ifndef OIL_TEMP_NEEDLE_GAUGE_H
#define OIL_TEMP_NEEDLE_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initialize oil temperature needle gauge (normal mode)
 *
 * Creates a simple needle gauge with tick markers and value display
 * in the bottom right corner.
 */
void oil_temp_needle_gauge_init(void);

/**
 * @brief Set the oil temperature value
 *
 * @param temperature Oil temperature in Celsius
 */
void oil_temp_needle_gauge_set_value(int32_t temperature);

#ifdef __cplusplus
}
#endif

#endif // OIL_TEMP_NEEDLE_GAUGE_H

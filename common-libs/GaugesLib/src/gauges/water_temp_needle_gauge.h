#ifndef WATER_TEMP_NEEDLE_GAUGE_H
#define WATER_TEMP_NEEDLE_GAUGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initialize water temperature needle gauge (normal mode)
 *
 * Creates a simple needle gauge with tick markers and value display
 * in the bottom right corner.
 */
void water_temp_needle_gauge_init(void);

/**
 * @brief Set the water temperature value
 *
 * @param temperature Water temperature in Celsius
 */
void water_temp_needle_gauge_set_value(int32_t temperature);

#ifdef __cplusplus
}
#endif

#endif // WATER_TEMP_NEEDLE_GAUGE_H

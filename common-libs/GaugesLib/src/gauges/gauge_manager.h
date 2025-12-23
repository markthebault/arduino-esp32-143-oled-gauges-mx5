#ifndef GAUGE_MANAGER_H
#define GAUGE_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

/**
 * @brief Enum for available gauges
 */
typedef enum {
    GAUGE_OIL_TEMP = 0,
    GAUGE_WATER_TEMP = 1,
    GAUGE_MULTI = 2,
    GAUGE_COUNT  // Total number of gauges
} gauge_type_t;

/**
 * @brief Initialize the gauge manager and all gauges
 *
 * Initializes all gauges but only makes the first one visible.
 * Should be called once during setup after LVGL is initialized.
 */
void gauge_manager_init(void);

/**
 * @brief Switch to the next gauge
 *
 * Hides the current gauge and shows the next one in the sequence.
 * Wraps around to the first gauge after the last one.
 */
void gauge_manager_next(void);

/**
 * @brief Get the currently active gauge
 *
 * @return gauge_type_t The currently visible gauge
 */
gauge_type_t gauge_manager_get_current(void);

/**
 * @brief Update the current gauge with new data
 *
 * @param oilTemp Oil temperature value
 * @param waterTemp Water temperature value
 * @param oilPressure Oil pressure value in bar
 * @param rpm Engine RPM (used for dynamic oil pressure thresholds)
 *
 * Updates only the currently visible gauge based on the type.
 */
void gauge_manager_update(float oilTemp, float waterTemp, float oilPressure, int32_t rpm);

/**
 * @brief Update gauges with animated test values
 *
 * For testing purposes. Animates both oil and water temperature gauges
 * through their full ranges in a continuous cycle:
 * - 0-9 seconds: Sweep up (oil: 60->160, water: 60->140)
 * - 9-12 seconds: Sweep down back to starting values
 *
 * Call this function repeatedly in your loop() for testing.
 */
void gauge_manager_update_test_animation(void);

#ifdef __cplusplus
}
#endif

#endif // GAUGE_MANAGER_H

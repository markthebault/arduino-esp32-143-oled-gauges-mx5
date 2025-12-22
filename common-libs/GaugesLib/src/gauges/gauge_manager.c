#ifdef __cplusplus
extern "C" {
#endif

#include "gauge_manager.h"
#include "gauges_config.h"
#include "oil_temp_gauge.h"
#include "water_temp_gauge.h"

// For millis() function
#ifdef ARDUINO
#include "Arduino.h"
#else
// For non-Arduino platforms, provide a fallback
extern unsigned long millis(void);
#endif

// ============================================================================
// PRIVATE STATE
// ============================================================================

static gauge_type_t current_gauge = DEFAULT_GAUGE;
static lv_obj_t *gauge_screens[GAUGE_COUNT] = {NULL};

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void gauge_manager_init(void) {
    // Create screen for oil temp gauge
    gauge_screens[GAUGE_OIL_TEMP] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(gauge_screens[GAUGE_OIL_TEMP], lv_color_hex(0x000000), 0);
    lv_scr_load(gauge_screens[GAUGE_OIL_TEMP]);
    oil_temp_gauge_init();

    // Create screen for water temp gauge
    gauge_screens[GAUGE_WATER_TEMP] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(gauge_screens[GAUGE_WATER_TEMP], lv_color_hex(0x000000), 0);
    lv_scr_load(gauge_screens[GAUGE_WATER_TEMP]);
    water_temp_gauge_init();

    // Load the default gauge (configured via DEFAULT_GAUGE build flag)
    lv_scr_load(gauge_screens[DEFAULT_GAUGE]);
    current_gauge = DEFAULT_GAUGE;
}

void gauge_manager_next(void) {
    // Move to next gauge, wrapping around
    current_gauge = (gauge_type_t)((current_gauge + 1) % GAUGE_COUNT);

    // Load the screen for the new gauge
    if (gauge_screens[current_gauge] != NULL) {
        lv_scr_load(gauge_screens[current_gauge]);
    }
}

gauge_type_t gauge_manager_get_current(void) {
    return current_gauge;
}

void gauge_manager_update(float oilTemp, float waterTemp) {
    // Update the appropriate gauge based on which one is currently visible
    switch (current_gauge) {
        case GAUGE_OIL_TEMP:
            oil_temp_gauge_set_value((int32_t)oilTemp);
            break;
        case GAUGE_WATER_TEMP:
            water_temp_gauge_set_value((int32_t)waterTemp);
            break;
        default:
            break;
    }
}

void gauge_manager_update_test_animation(void) {
    const unsigned long period = 12000UL;
    unsigned long t = millis() % period;

    int32_t oil_temp;
    int32_t water_temp;

    if (t < 9000UL) {
        // --- Sweep Up (0 to 9 seconds) ---

        // Oil: 60 -> 160 (Range of 100)
        oil_temp = 60 + (int32_t)((100UL * t) / 9000UL);

        // Water: 60 -> 140 (Range of 80)
        // We use 80UL here so it hits exactly 140 at the 9-second mark
        water_temp = 60 + (int32_t)((80UL * t) / 9000UL);
    }
    else {
        // --- Sweep Down (9 to 12 seconds) ---
        unsigned long t2 = t - 9000UL;

        // Oil: 160 -> 60
        oil_temp = 160 - (int32_t)((100UL * t2) / 3000UL);

        // Water: 140 -> 60 (Range of 80)
        // Subtracted from 140 so the animation is fluid
        water_temp = 140 - (int32_t)((80UL * t2) / 3000UL);
    }

    gauge_manager_update(oil_temp, water_temp);
}

#ifdef __cplusplus
}
#endif

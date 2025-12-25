#ifdef __cplusplus
extern "C" {
#endif

#include "gauge_manager.h"
#include "gauges_config.h"
#include "oil_temp_gauge.h"
#include "water_temp_gauge.h"
#include "multi_gauge.h"
#include "oil_pressure_gauge.h"
#include "oil_temp_needle_gauge.h"
#include "water_temp_needle_gauge.h"
#include "oil_pressure_needle_gauge.h"

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
static lv_obj_t *needle_gauge_screens[GAUGE_COUNT] = {NULL};
static gauge_gesture_callback_t gesture_callback = NULL;
static uint8_t current_gauge_mode = 1;  // 0 = normal/needle, 1 = racing/arc (default to racing)

// ============================================================================
// PRIVATE FUNCTIONS
// ============================================================================

/**
 * @brief Internal gesture event handler
 *
 * Detects swipe gestures and switches gauges:
 * - Right swipe: next gauge
 * - Left swipe: previous gauge
 */
static void internal_gesture_handler(lv_event_t * e) {
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

    if (dir == LV_DIR_RIGHT) {
        gauge_manager_next();
    } else if (dir == LV_DIR_LEFT) {
        gauge_manager_previous();
    }
}

// ============================================================================
// PUBLIC API IMPLEMENTATION
// ============================================================================

void gauge_manager_init(void) {
    // ========== RACING MODE GAUGES (ARC STYLE) ==========

    // Create screen for oil temp gauge (racing)
    gauge_screens[GAUGE_OIL_TEMP] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(gauge_screens[GAUGE_OIL_TEMP], COLOR_SCREEN_BG, 0);
    lv_scr_load(gauge_screens[GAUGE_OIL_TEMP]);
    oil_temp_gauge_init();

    // Create screen for water temp gauge (racing)
    gauge_screens[GAUGE_WATER_TEMP] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(gauge_screens[GAUGE_WATER_TEMP], COLOR_SCREEN_BG, 0);
    lv_scr_load(gauge_screens[GAUGE_WATER_TEMP]);
    water_temp_gauge_init();

    // Create screen for multi gauge (racing)
    gauge_screens[GAUGE_MULTI] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(gauge_screens[GAUGE_MULTI], COLOR_SCREEN_BG, 0);
    lv_scr_load(gauge_screens[GAUGE_MULTI]);
    multi_gauge_init();

    // Create screen for oil pressure gauge (racing)
    gauge_screens[GAUGE_OIL_PRESSURE] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(gauge_screens[GAUGE_OIL_PRESSURE], COLOR_SCREEN_BG, 0);
    lv_scr_load(gauge_screens[GAUGE_OIL_PRESSURE]);
    oil_pressure_gauge_init();

    // ========== NORMAL MODE GAUGES (NEEDLE STYLE) ==========

    // Create screen for oil temp needle gauge (normal)
    needle_gauge_screens[GAUGE_OIL_TEMP] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(needle_gauge_screens[GAUGE_OIL_TEMP], COLOR_SCREEN_BG, 0);
    lv_scr_load(needle_gauge_screens[GAUGE_OIL_TEMP]);
    oil_temp_needle_gauge_init();

    // Create screen for water temp needle gauge (normal)
    needle_gauge_screens[GAUGE_WATER_TEMP] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(needle_gauge_screens[GAUGE_WATER_TEMP], COLOR_SCREEN_BG, 0);
    lv_scr_load(needle_gauge_screens[GAUGE_WATER_TEMP]);
    water_temp_needle_gauge_init();

    // Create screen for oil pressure needle gauge (normal)
    needle_gauge_screens[GAUGE_OIL_PRESSURE] = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(needle_gauge_screens[GAUGE_OIL_PRESSURE], COLOR_SCREEN_BG, 0);
    lv_scr_load(needle_gauge_screens[GAUGE_OIL_PRESSURE]);
    oil_pressure_needle_gauge_init();

    // Note: Multi gauge is only available in racing mode
    needle_gauge_screens[GAUGE_MULTI] = NULL;

    // Load the default gauge (configured via DEFAULT_GAUGE build flag)
    // Start with racing mode by default
    lv_scr_load(gauge_screens[DEFAULT_GAUGE]);
    current_gauge = DEFAULT_GAUGE;
    current_gauge_mode = 1;  // Racing mode
}

void gauge_manager_next(void) {
    // Move to next gauge, wrapping around
    current_gauge = (gauge_type_t)((current_gauge + 1) % GAUGE_COUNT);

    // Skip multi gauge if in normal mode (needle gauges don't have multi gauge)
    if (current_gauge_mode == 0 && current_gauge == GAUGE_MULTI) {
        current_gauge = (gauge_type_t)((current_gauge + 1) % GAUGE_COUNT);
    }

    // Load the screen for the new gauge based on mode
    if (current_gauge_mode == 1) {
        // Racing mode - use arc gauges
        if (gauge_screens[current_gauge] != NULL) {
            lv_scr_load(gauge_screens[current_gauge]);
        }
    } else {
        // Normal mode - use needle gauges
        if (needle_gauge_screens[current_gauge] != NULL) {
            lv_scr_load(needle_gauge_screens[current_gauge]);
        }
    }
}

void gauge_manager_previous(void) {
    // Move to previous gauge, wrapping around
    if (current_gauge == 0) {
        current_gauge = (gauge_type_t)(GAUGE_COUNT - 1);
    } else {
        current_gauge = (gauge_type_t)(current_gauge - 1);
    }

    // Skip multi gauge if in normal mode (needle gauges don't have multi gauge)
    if (current_gauge_mode == 0 && current_gauge == GAUGE_MULTI) {
        if (current_gauge == 0) {
            current_gauge = (gauge_type_t)(GAUGE_COUNT - 1);
        } else {
            current_gauge = (gauge_type_t)(current_gauge - 1);
        }
    }

    // Load the screen for the new gauge based on mode
    if (current_gauge_mode == 1) {
        // Racing mode - use arc gauges
        if (gauge_screens[current_gauge] != NULL) {
            lv_scr_load(gauge_screens[current_gauge]);
        }
    } else {
        // Normal mode - use needle gauges
        if (needle_gauge_screens[current_gauge] != NULL) {
            lv_scr_load(needle_gauge_screens[current_gauge]);
        }
    }
}

gauge_type_t gauge_manager_get_current(void) {
    return current_gauge;
}

void gauge_manager_enable_gestures(void) {
    // Attach the internal gesture handler to all gauge screens (racing mode)
    for (int i = 0; i < GAUGE_COUNT; i++) {
        if (gauge_screens[i] != NULL) {
            lv_obj_add_event_cb(gauge_screens[i], internal_gesture_handler, LV_EVENT_GESTURE, NULL);
        }
    }

    // Attach the internal gesture handler to all needle gauge screens (normal mode)
    for (int i = 0; i < GAUGE_COUNT; i++) {
        if (needle_gauge_screens[i] != NULL) {
            lv_obj_add_event_cb(needle_gauge_screens[i], internal_gesture_handler, LV_EVENT_GESTURE, NULL);
        }
    }
}

void gauge_manager_set_gesture_callback(gauge_gesture_callback_t callback) {
    gesture_callback = callback;

    // Attach the custom gesture callback to all gauge screens (racing mode)
    for (int i = 0; i < GAUGE_COUNT; i++) {
        if (gauge_screens[i] != NULL) {
            lv_obj_add_event_cb(gauge_screens[i], callback, LV_EVENT_GESTURE, NULL);
        }
    }

    // Attach the custom gesture callback to all needle gauge screens (normal mode)
    for (int i = 0; i < GAUGE_COUNT; i++) {
        if (needle_gauge_screens[i] != NULL) {
            lv_obj_add_event_cb(needle_gauge_screens[i], callback, LV_EVENT_GESTURE, NULL);
        }
    }
}

void gauge_manager_update(float oilTemp, float waterTemp, float oilPressure, int32_t rpm, uint8_t gaugeMode) {
    // Update gauge mode if it has changed
    if (gaugeMode != current_gauge_mode) {
        current_gauge_mode = gaugeMode;

        // Switch to the appropriate screen for the new mode
        if (current_gauge_mode == 1) {
            // Racing mode - load arc gauge
            if (gauge_screens[current_gauge] != NULL) {
                lv_scr_load(gauge_screens[current_gauge]);
            }
        } else {
            // Normal mode - load needle gauge
            // Skip multi gauge if in normal mode
            if (current_gauge == GAUGE_MULTI) {
                current_gauge = GAUGE_OIL_TEMP;  // Default to oil temp
            }
            if (needle_gauge_screens[current_gauge] != NULL) {
                lv_scr_load(needle_gauge_screens[current_gauge]);
            }
        }
    }

    // Update the appropriate gauge based on mode and current gauge type
    if (current_gauge_mode == 1) {
        // Racing mode - update arc gauges
        switch (current_gauge) {
            case GAUGE_OIL_TEMP:
                oil_temp_gauge_set_value((int32_t)oilTemp);
                break;
            case GAUGE_WATER_TEMP:
                water_temp_gauge_set_value((int32_t)waterTemp);
                break;
            case GAUGE_MULTI:
                multi_gauge_set_values((int32_t)waterTemp, (int32_t)oilTemp, oilPressure, rpm);
                break;
            case GAUGE_OIL_PRESSURE:
                oil_pressure_gauge_set_value(oilPressure, rpm);
                break;
            default:
                break;
        }
    } else {
        // Normal mode - update needle gauges
        switch (current_gauge) {
            case GAUGE_OIL_TEMP:
                oil_temp_needle_gauge_set_value((int32_t)oilTemp);
                break;
            case GAUGE_WATER_TEMP:
                water_temp_needle_gauge_set_value((int32_t)waterTemp);
                break;
            case GAUGE_OIL_PRESSURE:
                oil_pressure_needle_gauge_set_value(oilPressure, rpm);
                break;
            case GAUGE_MULTI:
                // Multi gauge not available in normal mode, skip
                break;
            default:
                break;
        }
    }
}

void gauge_manager_update_test_animation(void) {
    const unsigned long period = 12000UL;
    unsigned long t = millis() % period;

    int32_t oil_temp;
    int32_t water_temp;
    float oil_pressure;
    int32_t rpm;

    if (t < 9000UL) {
        // --- Sweep Up (0 to 9 seconds) ---

        // Oil: 60 -> 160 (Range of 100)
        oil_temp = 60 + (int32_t)((100UL * t) / 9000UL);

        // Water: 60 -> 140 (Range of 80)
        // We use 80UL here so it hits exactly 140 at the 9-second mark
        water_temp = 60 + (int32_t)((80UL * t) / 9000UL);

        // Oil Pressure: 0 -> 8 bar (Range of 8)
        oil_pressure = (float)((80UL * t) / 9000UL) / 10.0f;

        // RPM: 800 -> 6000 (Range of 5200)
        // rpm = 800 + (int32_t)((5200UL * t) / 9000UL);
        rpm = 4000;
    }
    else {
        // --- Sweep Down (9 to 12 seconds) ---
        unsigned long t2 = t - 9000UL;

        // Oil: 160 -> 60
        oil_temp = 160 - (int32_t)((100UL * t2) / 3000UL);

        // Water: 140 -> 60 (Range of 80)
        // Subtracted from 140 so the animation is fluid
        water_temp = 140 - (int32_t)((80UL * t2) / 3000UL);

        // Oil Pressure: 8 -> 0 bar
        oil_pressure = 8.0f - (float)((80UL * t2) / 3000UL) / 10.0f;

        // RPM: 6000 -> 800
        rpm = 6000 - (int32_t)((5200UL * t2) / 3000UL);
    }

    gauge_manager_update(oil_temp, water_temp, oil_pressure, rpm, 1);  // Use racing mode for test
}

#ifdef __cplusplus
}
#endif

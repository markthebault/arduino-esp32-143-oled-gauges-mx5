#ifdef __cplusplus
extern "C" {
#endif

#include "gauge_manager.h"
#include "oil_temp_gauge.h"
#include "water_temp_gauge.h"

// ============================================================================
// PRIVATE STATE
// ============================================================================

static gauge_type_t current_gauge = GAUGE_OIL_TEMP;
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

    // Load the first gauge (oil temp)
    lv_scr_load(gauge_screens[GAUGE_OIL_TEMP]);
    current_gauge = GAUGE_OIL_TEMP;
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

#ifdef __cplusplus
}
#endif

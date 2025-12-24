#include "./display/lcd_bsp.h"
#include "./touch/FT3168.h"
#include "gauges/gauge_manager.h"
#include "./communication/esp_now_receiver.h"

// Double-tap detection constants
#define DOUBLE_TAP_INTERVAL_MS 500  // Maximum time between taps
#define TAP_DEBOUNCE_MS 40         // Minimum time between taps to avoid bounce

// Double-tap state
static unsigned long last_tap_time = 0;
static bool waiting_for_second_tap = false;

void setup() {
  Serial.begin(115200);
  delay(2000); // Give serial time to start
  Serial.println("Starting ESP32 OLED Gauges...");

  // 1. Initialize Display & LVGL
  Touch_Init();
  lcd_lvgl_Init();

  // 2. Initialize all gauges via the gauge manager
  if (example_lvgl_lock(-1)) {
    gauge_manager_init();
    example_lvgl_unlock();
  }

  // Note: Performance monitor is automatically created by LVGL 8 when LV_USE_PERF_MONITOR is enabled
  // Position is set by LV_USE_PERF_MONITOR_POS in lv_conf.h

  // 2. Initialize ESP-NOW Receiver (Wi-Fi + ESP-NOW)
  espnow_receiver_init();
}

void loop() {
  // Check for touch input and detect double-tap
  uint16_t touch_x, touch_y;
  if (getTouch(&touch_x, &touch_y)) {
    unsigned long current_time = millis();
    unsigned long time_since_last_tap = current_time - last_tap_time;

    // Check if this is a second tap within the double-tap interval
    if (waiting_for_second_tap &&
        time_since_last_tap >= TAP_DEBOUNCE_MS &&
        time_since_last_tap <= DOUBLE_TAP_INTERVAL_MS) {

      // Double-tap detected! Switch to next gauge
      Serial.println("Double-tap detected! Switching gauge...");
      if (example_lvgl_lock(-1)) {
        gauge_manager_next();
        example_lvgl_unlock();
      }

      waiting_for_second_tap = false;
      last_tap_time = 0;
    }
    else if (time_since_last_tap >= TAP_DEBOUNCE_MS) {
      // First tap or tap after timeout
      waiting_for_second_tap = true;
      last_tap_time = current_time;
    }
  }
  else {
    // No touch detected - reset double-tap if interval expired
    if (waiting_for_second_tap && (millis() - last_tap_time) > DOUBLE_TAP_INTERVAL_MS) {
      waiting_for_second_tap = false;
    }
  }

  // Update the current gauge if we have received data
  if (dataReceived) {
    if (example_lvgl_lock(-1)) {
      Serial.print("DEBUG - oilTemp: ");
      Serial.print(latestData.oilTemp);
      Serial.print(", waterTemp: ");
      Serial.print(latestData.waterTemp);
      Serial.print(", oilPressure: ");
      Serial.print(latestData.oilPressure);
      Serial.print(", RPM: ");
      Serial.println(latestData.engineRPM);
      
      gauge_manager_update(latestData.oilTemp, latestData.waterTemp, latestData.oilPressure, latestData.engineRPM);
      example_lvgl_unlock();
    }
  }

  // This is for testing the oil temp gauge animation when ESP-NOW is not used
  // if (example_lvgl_lock(-1)) {
  //   gauge_manager_update_test_animation();
  //   example_lvgl_unlock();
  // }

    // if (example_lvgl_lock(-1)) {
    //   gauge_manager_update(120, 80); //TODO: Use oil temp to test
    //   example_lvgl_unlock();
    // }

  delay(200);
}

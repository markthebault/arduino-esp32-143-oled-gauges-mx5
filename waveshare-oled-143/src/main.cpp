#include "./display/lcd_bsp.h"
#include "./touch/FT3168.h"
#include "gauges/gauge_manager.h"
#include "./communication/esp_now_receiver.h"

// Gesture event handler for swipe detection
static void gesture_event_handler(lv_event_t * e) {
  lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());

  if (dir == LV_DIR_RIGHT) {
    Serial.println("Swipe right detected! Switching gauge...");
    gauge_manager_next();

    // Re-attach event handler to the new screen
    lv_obj_t *screen = lv_scr_act();
    lv_obj_add_event_cb(screen, gesture_event_handler, LV_EVENT_GESTURE, NULL);
  }
}

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

    // Add gesture event handler to the current screen for swipe detection
    lv_obj_t *screen = lv_scr_act();
    lv_obj_add_event_cb(screen, gesture_event_handler, LV_EVENT_GESTURE, NULL);

    example_lvgl_unlock();
  }

  // Note: Performance monitor is automatically created by LVGL 8 when LV_USE_PERF_MONITOR is enabled
  // Position is set by LV_USE_PERF_MONITOR_POS in lv_conf.h

  // 3. Initialize ESP-NOW Receiver (Wi-Fi + ESP-NOW)
  espnow_receiver_init();
}

void loop() {
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

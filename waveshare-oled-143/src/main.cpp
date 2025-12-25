#include "./display/lcd_bsp.h"
#include "./touch/FT3168.h"
#include "gauges/gauge_manager.h"
#include <esp_now_receiver.h>

void setup() {
  Serial.begin(115200);
  delay(2000); // Give serial time to start
  Serial.println("Starting ESP32 OLED Gauges...");

  // 1. Initialize Display & LVGL
  Touch_Init();
  lcd_lvgl_Init();

  // Set brightness (0-255)
  lcd_set_brightness(0xFF);  // Maximum brightness (100%)
  // lcd_set_brightness(0x80);  // Medium brightness (50%)
  // lcd_set_brightness(0x40);  // Low brightness (25%)
  // lcd_set_brightness(0x00);  // Minimum brightness (off)

  // 2. Initialize all gauges via the gauge manager
  if (example_lvgl_lock(-1)) {
    gauge_manager_init();
    gauge_manager_enable_gestures();  // Enable swipe gesture to switch gauges
    example_lvgl_unlock();
  }

  // Note: Performance monitor is automatically created by LVGL 8 when LV_USE_PERF_MONITOR is enabled
  // Position is set by LV_USE_PERF_MONITOR_POS in lv_conf.h

  // 3. Initialize ESP-NOW Receiver (Wi-Fi + ESP-NOW)
  espnow_receiver_init();
}

void loop() {
  // Check timeout and update telemetry (sets values to -1 if no data in 5s)
  espnow_check_timeout();

  // Update the current gauge
  if (example_lvgl_lock(-1)) {
    Serial.print("DEBUG - oilTemp: ");
    Serial.print(latestData.oilTemp);
    Serial.print(", waterTemp: ");
    Serial.print(latestData.waterTemp);
    Serial.print(", oilPressure: ");
    Serial.print(latestData.oilPressure);
    Serial.print(", RPM: ");
    Serial.println(latestData.engineRPM);

    gauge_manager_update(latestData.oilTemp, latestData.waterTemp, latestData.oilPressure, latestData.engineRPM, latestData.gaugeType);
    example_lvgl_unlock();
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

#include "src/display/lcd_bsp.h"
#include "src/touch/FT3168.h"
#include "src/gauges/mth_gauges.h"
#include "src/communication/esp_now_receiver.h"

void setup() {
  Serial.begin(115200);
  delay(2000); // Give serial time to start

  // 1. Initialize Display & LVGL
  Touch_Init();
  lcd_lvgl_Init();

  if (example_lvgl_lock(-1)) {
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0);
    mth_gauge_oil_temp_init();
    example_lvgl_unlock();
  }

  // 2. Initialize ESP-NOW Receiver (Wi-Fi + ESP-NOW)
  espnow_receiver_init();
}

void loop() {
  // Only update the gauge if we have received at least one packet
  if (dataReceived) {
    if (example_lvgl_lock(-1)) {
      // Cast the float to the expected int32_t for the gauge
      mth_gauge_set_temp((int32_t)latestData.oilTemp);
      example_lvgl_unlock();
    }
  }

  delay(30); 
}

/////////////// Test animation function when espnow is not used ///////////////
void set_oil_temp_animation() {
  const unsigned long period = 12000UL;
  unsigned long t = millis() % period;

  int32_t temp_val;

  if (t < 9000UL) {
    // Sweep Up (60 to 160)
    temp_val = 60 + (int32_t)((100UL * t) / 9000UL);
  } else {
    // Sweep Down (160 to 60)
    unsigned long t2 = t - 9000UL;
    temp_val = 160 - (int32_t)((100UL * t2) / 3000UL);
  }

  // Update oil temperature gauge
  mth_gauge_set_temp(temp_val);
}
/////////////////////////////////////////////////////
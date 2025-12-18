#include "lcd_bsp.h"
#include "FT3168.h"
#include "mth_gauges.h"

void set_speed_animation();
void set_oil_animation();


void setup()
{
  Serial.begin(115200);
  Touch_Init();
  lcd_lvgl_Init();
}

void loop()
{
  // Replicate the original LVGL animation in the loop:
  // set_speed_animation();
  set_oil_animation();

  // Small delay to make updates smooth (~50Hz)
  delay(20);
}






/////////////// Custom Loop Functions ///////////////
void set_oil_animation() {
  const unsigned long period = 4000UL; 
  unsigned long t = millis() % period;
  
  float press_val;
  int32_t temp_val;

  if (t < 3000UL) {
    // Sweep Up (0 to Max)
    press_val = (6.0f * t) / 3000.0f;
    temp_val = (int32_t)((180UL * t) / 3000UL);
  } else {
    // Sweep Down (Max to 0)
    unsigned long t2 = t - 3000UL;
    press_val = 6.0f - (6.0f * t2 / 1000.0f);
    temp_val = (int32_t)(180UL - (180UL * t2) / 1000UL);
  }

  // Update gauges
  mth_gauge_set_pressure(press_val);
  mth_gauge_set_temp(temp_val);
}


void set_speed_animation()
{
  // - forward: 0 -> 240 over 3000 ms
  // - playback: 240 -> 0 over 1000 ms
  // - repeat infinitely (total period = 4000 ms)
  const unsigned long period = 4000UL; // 3000 + 1000
  unsigned long t = millis() % period;
  int32_t value;

  if (t < 3000UL) {
    // Linear interpolate 0..240 over 3000ms
    value = (int32_t)((240UL * t) / 3000UL);
  } else {
    // Linear interpolate 240..0 over 1000ms
    unsigned long t2 = t - 3000UL;
    value = (int32_t)(240UL - (240UL * t2) / 1000UL);
  }

  // Update the gauge directly (replaces LVGL animation)
  mth_gauge_set_speed(value);

  // Optional serial output for debugging
  Serial.print("gauge value: ");
  Serial.println(value);
}
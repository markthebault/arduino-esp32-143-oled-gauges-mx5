#include "lcd_bsp.h"
#include "FT3168.h"
#include "mth_gauges.h"

void set_speed_animation();


void setup()
{
  Serial.begin(115200);
  Touch_Init();
  lcd_lvgl_Init();

  // Create the gauge (initializes meter and indicator)
  mth_gauge_speedometer_dark();
}

void loop()
{
  // Replicate the original LVGL animation in the loop:
  set_speed_animation();

  // Small delay to make updates smooth (~50Hz)
  delay(20);
}






/////////////// Custom Loop Functions ///////////////
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
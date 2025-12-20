#include "lcd_bsp.h"
#include "FT3168.h"
#include "mth_gauges.h"

void set_oil_temp_animation();


void setup()
{
  Serial.begin(115200);
  Touch_Init();
  lcd_lvgl_Init();

  // Custom configuration
  if (example_lvgl_lock(-1)) 
  {   
    // Set the screen in Dark. Not sure if here is the correct place to put this
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), 0); 

    mth_gauge_oil_temp_init();

    // mth_gauge_set_temp(135);
    
    // Release the mutex
    example_lvgl_unlock();
  }
}

void loop()
{
  if (example_lvgl_lock(-1))
  {
    set_oil_temp_animation();
    example_lvgl_unlock();
  }

  // Small delay to make updates smooth (~50Hz)
  delay(20);
}






/////////////// Custom Loop Functions ///////////////
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
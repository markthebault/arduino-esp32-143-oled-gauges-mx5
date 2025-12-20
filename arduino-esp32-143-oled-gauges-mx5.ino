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
    
    // Release the mutex
    example_lvgl_unlock();
  }
}

void loop()
{
  set_oil_temp_animation();

  // Small delay to make updates smooth (~50Hz)
  delay(20);
}






/////////////// Custom Loop Functions ///////////////
void set_oil_temp_animation() {
  const unsigned long period = 4000UL;
  unsigned long t = millis() % period;

  int32_t temp_val;

  if (t < 3000UL) {
    // Sweep Up (0 to 180)
    temp_val = (int32_t)((180UL * t) / 3000UL);
  } else {
    // Sweep Down (180 to 0)
    unsigned long t2 = t - 3000UL;
    temp_val = (int32_t)(180UL - (180UL * t2) / 1000UL);
  }

  // Update oil temperature gauge
  mth_gauge_set_temp(temp_val);
}
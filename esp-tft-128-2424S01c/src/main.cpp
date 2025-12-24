#include "Arduino.h"
#include <lvgl.h>
#include <Ticker.h>
#include "CST816D.h"

#include "display/lcd_bsp.h"
#include "display/lcd_config.h"
#include "gauges/gauge_manager.h"
#include <esp_now_receiver.h>

// Touch Pin Definitions
#define I2C_SDA 4
#define I2C_SCL 5
#define TP_INT 0
#define TP_RST 1

#define off_pin 35

// Create touch instance
CST816D touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT);

#if LV_USE_LOG != 0
/* Serial debugging for LVGL */
void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *fn_name, const char *dsc)
{
  Serial.printf("%s(%s)@%d->%s\r\n", file, fn_name, line, dsc);
  Serial.flush();
}
#endif

/* Touchpad reading: Interface between LVGL and the touch driver */
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
  bool touched;
  uint8_t gesture;
  uint16_t touchX, touchY;

  touched = touch.getTouch(&touchX, &touchY, &gesture);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
  else
  {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = touchX;
    data->point.y = touchY;
  }
}

Ticker ticker;

// Periodic function to check memory usage
void tcr1s()
{
  Serial.printf("SRAM free size: %d\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
}

void setup()
{
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  
  Serial.begin(115200); 
  delay(2000); // Give serial time to start
  Serial.println("I am LVGL_Arduino");

  ticker.attach(1, tcr1s); // Print memory status every 1 second

  // Initialize LVGL
  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* Register debug function */
#endif

  // Initialize LCD display (includes LVGL driver registration)
  lcd_init();

  // Initialize touch
  touch.begin();

  /* Initialize the touch input device driver */
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  // Initialize your custom gauge manager
  gauge_manager_init();
  gauge_manager_enable_gestures();

  espnow_receiver_init();

  // Note: Performance monitor is automatically created by LVGL 8 when LV_USE_PERF_MONITOR is enabled
  // Position is set by LV_USE_PERF_MONITOR_POS in lv_conf.h

  Serial.println("Setup done");
}

void loop()
{
  lv_timer_handler(); /* Process LVGL timers and GUI updates */

  if (dataReceived) {
    Serial.print("DEBUG - oilTemp: ");
    Serial.print(latestData.oilTemp);
    Serial.print(", waterTemp: ");
    Serial.print(latestData.waterTemp);
    Serial.print(", oilPressure: ");
    Serial.print(latestData.oilPressure);
    Serial.print(", RPM: ");
    Serial.println(latestData.engineRPM);

    gauge_manager_update(latestData.oilTemp, latestData.waterTemp, latestData.oilPressure, latestData.engineRPM);
  }

  // Uncomment for testing without ESP-NOW
  // gauge_manager_update_test_animation(); // Update gauge values with test animation

  delay(70); // Small delay to prevent hogging CPU
}
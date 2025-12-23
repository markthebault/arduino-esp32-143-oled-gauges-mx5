#define LGFX_USE_V1

#include "Arduino.h"
#include <lvgl.h>
#include <LovyanGFX.hpp>
#include <Ticker.h>
#include "CST816D.h"

#include "gauges/gauge_manager.h"

// Hardware Pin Definitions
#define I2C_SDA 4
#define I2C_SCL 5
#define TP_INT 0
#define TP_RST 1

#define off_pin 35
#define buf_size 100

// LovyanGFX Configuration Class
class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      // SPI Bus Settings
      cfg.spi_host = SPI2_HOST;         // Select SPI (ESP32-S2/C3: SPI2_HOST or SPI3_HOST)
      cfg.spi_mode = 0;                  // SPI Mode (0 ~ 3)
      cfg.freq_write = 80000000;         // SPI Clock for writing (Max 80MHz)
      cfg.freq_read = 20000000;          // SPI Clock for reading
      cfg.spi_3wire = true;              // Set true if receiving via MOSI pin
      cfg.use_lock = true;               // Set true to use transaction locks
      cfg.dma_channel = SPI_DMA_CH_AUTO; // Set DMA channel (Auto recommended for latest ESP-IDF)
      
      cfg.pin_sclk = 6;                  // SPI SCLK pin
      cfg.pin_mosi = 7;                  // SPI MOSI pin
      cfg.pin_miso = -1;                 // SPI MISO pin (-1 to disable)
      cfg.pin_dc = 2;                    // SPI D/C pin

      _bus_instance.config(cfg);              // Apply bus settings
      _panel_instance.setBus(&_bus_instance); // Connect bus to panel
    }

    {
      auto cfg = _panel_instance.config(); // Get structure for display panel settings

      cfg.pin_cs = 10;   // CS pin (-1 to disable)
      cfg.pin_rst = -1;  // RST pin (-1 to disable)
      cfg.pin_busy = -1; // BUSY pin (-1 to disable)

      // Panel Dimensions and Offsets
      cfg.memory_width = 240;   // Max width supported by Driver IC
      cfg.memory_height = 240;  // Max height supported by Driver IC
      cfg.panel_width = 240;    // Actual displayable width
      cfg.panel_height = 240;   // Actual displayable height
      cfg.offset_x = 0;         // X offset
      cfg.offset_y = 0;         // Y offset
      
      cfg.offset_rotation = 0;  // Rotation offset 0~7
      cfg.dummy_read_pixel = 8; // Dummy bits before reading pixels
      cfg.dummy_read_bits = 1;  // Dummy bits before reading non-pixel data
      cfg.readable = false;     // Set true if data can be read from the panel
      cfg.invert = true;        // Set true if colors are inverted
      cfg.rgb_order = false;    // Set true if Red and Blue are swapped
      cfg.dlen_16bit = false;   // Set true for panels sending data in 16-bit units
      cfg.bus_shared = false;   // Set true if SPI bus is shared (e.g., with SD card)

      _panel_instance.config(cfg);
    }

    setPanel(&_panel_instance); // Register the panel
  }
};

// Create instances
LGFX tft;
CST816D touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT);

/* Screen Resolution Settings */
static const uint32_t screenWidth = 240;
static const uint32_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][screenWidth * buf_size];

#if LV_USE_LOG != 0
/* Serial debugging for LVGL */
void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *fn_name, const char *dsc)
{
  Serial.printf("%s(%s)@%d->%s\r\n", file, fn_name, line, dsc);
  Serial.flush();
}
#endif

/* Display flushing: Interface between LVGL and the graphics library */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }

  // Use DMA for faster image pushing
  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);

  lv_disp_flush_ready(disp); /* Signal LVGL that flushing is finished */
}

/* Touchpad reading: Interface between LVGL and the touch driver */
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  bool touched;
  uint8_t gesture;
  uint16_t touchX, touchY;

  touched = touch.getTouch(&touchX, &touchY, &gesture);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;
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
  Serial.println("I am LVGL_Arduino");

  ticker.attach(1, tcr1s); // Print memory status every 1 second

  tft.init();
  tft.initDMA();
  tft.startWrite();
 
  touch.begin();
  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* Register debug function */
#endif

  lv_disp_draw_buf_init(&draw_buf, buf[0], buf[1], screenWidth * buf_size);

  /* Initialize the display driver for LVGL */
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /* Initialize the touch input device driver */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  // Initialize your custom gauge manager
  gauge_manager_init();

  // Note: Performance monitor is automatically created by LVGL 8 when LV_USE_PERF_MONITOR is enabled
  // Position is set by LV_USE_PERF_MONITOR_POS in lv_conf.h

  Serial.println("Setup done");
}

void loop()
{
  lv_timer_handler(); /* Process LVGL timers and GUI updates */

  gauge_manager_update_test_animation(); // Update gauge values with test animation
  
  delay(70); // Small delay to prevent hogging CPU
}
#include "lcd_bsp.h"
#include "lcd_config.h"
#include <Arduino.h>

// LVGL display buffer
static lv_color_t buf[2][LCD_WIDTH * LCD_BUF_SIZE];
static lv_display_t *disp;

bool lcd_init(void)
{
  // Initialize the LovyanGFX display
  tft.init();
  tft.initDMA();
  tft.startWrite();

  // Create LVGL display
  disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);

  // Set up double buffering
  // NOTE: In LVGL 9, buffer size is specified in PIXELS, not bytes
  lv_display_set_buffers(disp, buf[0], buf[1], LCD_WIDTH * LCD_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

  // Set the flush callback
  lv_display_set_flush_cb(disp, lcd_flush_cb);

  return true;
}

void lcd_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p)
{
  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }

  // Use DMA for faster image pushing
  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)color_p);

  lv_display_flush_ready(disp); /* Signal LVGL that flushing is finished */
}

uint32_t lcd_get_width(void)
{
  return LCD_WIDTH;
}

uint32_t lcd_get_height(void)
{
  return LCD_HEIGHT;
}

#include "lcd_bsp.h"
#include "lcd_config.h"
#include <Arduino.h>

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][LCD_WIDTH * LCD_BUF_SIZE];

bool lcd_init(void)
{
  // Initialize the LovyanGFX display
  tft.init();
  tft.initDMA();
  tft.startWrite();

  // Initialize LVGL display buffer (double buffering)
  lv_disp_draw_buf_init(&draw_buf, buf[0], buf[1], LCD_WIDTH * LCD_BUF_SIZE);

  // Initialize the display driver for LVGL
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = lcd_flush_cb;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  return true;
}

void lcd_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }

  // Use DMA for faster image pushing
  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);

  lv_disp_flush_ready(disp); /* Signal LVGL that flushing is finished */
}

uint32_t lcd_get_width(void)
{
  return LCD_WIDTH;
}

uint32_t lcd_get_height(void)
{
  return LCD_HEIGHT;
}

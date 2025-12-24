#ifndef LCD_BSP_H
#define LCD_BSP_H

#include <lvgl.h>
#include "lcd_driver.h"

/**
 * Initialize the LCD display and LVGL integration
 * @return true on success, false on failure
 */
bool lcd_init(void);

/**
 * LVGL display flush callback
 * Pushes framebuffer data to the display using DMA
 */
void lcd_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p);

/**
 * Get the width of the display
 * @return Display width in pixels
 */
uint32_t lcd_get_width(void);

/**
 * Get the height of the display
 * @return Display height in pixels
 */
uint32_t lcd_get_height(void);

#endif // LCD_BSP_H

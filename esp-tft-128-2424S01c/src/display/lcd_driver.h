#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "lcd_config.h"

/**
 * LovyanGFX driver for GC9A01 240x240 round TFT display
 * Configured for ESP32-C3 with SPI interface
 */
class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Bus_SPI _bus_instance;

public:
  LGFX(void);
};

// Global display instance
extern LGFX tft;

#endif // LCD_DRIVER_H

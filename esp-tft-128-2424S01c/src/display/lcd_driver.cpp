#include "lcd_driver.h"

// Global display instance
LGFX tft;

LGFX::LGFX(void)
{
  {
    auto cfg = _bus_instance.config();

    // SPI Bus Settings
    cfg.spi_host = LCD_SPI_HOST;           // Select SPI (ESP32-S2/C3: SPI2_HOST or SPI3_HOST)
    cfg.spi_mode = 0;                      // SPI Mode (0 ~ 3)
    cfg.freq_write = LCD_SPI_FREQ_WRITE;   // SPI Clock for writing (Max 80MHz)
    cfg.freq_read = LCD_SPI_FREQ_READ;     // SPI Clock for reading
    cfg.spi_3wire = true;                  // Set true if receiving via MOSI pin
    cfg.use_lock = true;                   // Set true to use transaction locks
    cfg.dma_channel = SPI_DMA_CH_AUTO;     // Set DMA channel (Auto recommended for latest ESP-IDF)

    cfg.pin_sclk = LCD_PIN_SCLK;           // SPI SCLK pin
    cfg.pin_mosi = LCD_PIN_MOSI;           // SPI MOSI pin
    cfg.pin_miso = LCD_PIN_MISO;           // SPI MISO pin (-1 to disable)
    cfg.pin_dc = LCD_PIN_DC;               // SPI D/C pin

    _bus_instance.config(cfg);             // Apply bus settings
    _panel_instance.setBus(&_bus_instance); // Connect bus to panel
  }

  {
    auto cfg = _panel_instance.config();   // Get structure for display panel settings

    cfg.pin_cs = LCD_PIN_CS;               // CS pin (-1 to disable)
    cfg.pin_rst = LCD_PIN_RST;             // RST pin (-1 to disable)
    cfg.pin_busy = -1;                     // BUSY pin (-1 to disable)

    // Panel Dimensions and Offsets
    cfg.memory_width = LCD_WIDTH;          // Max width supported by Driver IC
    cfg.memory_height = LCD_HEIGHT;        // Max height supported by Driver IC
    cfg.panel_width = LCD_WIDTH;           // Actual displayable width
    cfg.panel_height = LCD_HEIGHT;         // Actual displayable height
    cfg.offset_x = 0;                      // X offset
    cfg.offset_y = 0;                      // Y offset

    cfg.offset_rotation = 0;               // Rotation offset 0~7
    cfg.dummy_read_pixel = 8;              // Dummy bits before reading pixels
    cfg.dummy_read_bits = 1;               // Dummy bits before reading non-pixel data
    cfg.readable = false;                  // Set true if data can be read from the panel
    cfg.invert = true;                     // Set true if colors are inverted
    cfg.rgb_order = false;                 // Set true if Red and Blue are swapped
    cfg.dlen_16bit = false;                // Set true for panels sending data in 16-bit units
    cfg.bus_shared = false;                // Set true if SPI bus is shared (e.g., with SD card)

    _panel_instance.config(cfg);
  }

  setPanel(&_panel_instance);              // Register the panel
}

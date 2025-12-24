#ifndef LCD_CONFIG_H
#define LCD_CONFIG_H

// Display Resolution
#define LCD_WIDTH  240
#define LCD_HEIGHT 240

// SPI Pin Configuration
#define LCD_PIN_SCLK  6
#define LCD_PIN_MOSI  7
#define LCD_PIN_MISO  -1  // Not used
#define LCD_PIN_DC    2
#define LCD_PIN_CS    10
#define LCD_PIN_RST   -1  // Not used

// SPI Configuration
#define LCD_SPI_HOST       SPI2_HOST
#define LCD_SPI_FREQ_WRITE 80000000  // 80MHz for writing
#define LCD_SPI_FREQ_READ  20000000  // 20MHz for reading

// LVGL Buffer Configuration
#define LCD_BUF_SIZE 40  // Buffer height in lines

#endif // LCD_CONFIG_H

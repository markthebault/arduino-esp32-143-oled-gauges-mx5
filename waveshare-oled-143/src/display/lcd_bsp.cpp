#include "lcd_bsp.h"
#include "esp_lcd_sh8601.h"
#include "lcd_config.h"
#include "../touch/FT3168.h"
#include "read_lcd_id_bsp.h"


static SemaphoreHandle_t lvgl_mux = NULL; //mutex semaphores
#define LCD_HOST    SPI2_HOST

#define EXAMPLE_Rotate_90
#define SH8601_ID 0x86
#define CO5300_ID 0xff
static uint8_t READ_LCD_ID = 0x00;
static esp_lcd_panel_io_handle_t g_io_handle = NULL; // Global panel I/O handle for brightness control 

static const sh8601_lcd_init_cmd_t sh8601_lcd_init_cmds[] = 
{
  {0x11, (uint8_t []){0x00}, 0, 120},
  {0x44, (uint8_t []){0x01, 0xD1}, 2, 0},
  {0x35, (uint8_t []){0x00}, 1, 0},
  {0x53, (uint8_t []){0x20}, 1, 10},
  {0x51, (uint8_t []){0x00}, 1, 10},
  {0x29, (uint8_t []){0x00}, 0, 10},
  {0x51, (uint8_t []){0xFF}, 1, 0},
  //{0x36, (uint8_t []){0x80}, 1, 0},
};
static const sh8601_lcd_init_cmd_t co5300_lcd_init_cmds[] = 
{
  {0x11, (uint8_t []){0x00}, 0, 80},   
  {0xC4, (uint8_t []){0x80}, 1, 0},
  //{0x44, (uint8_t []){0x01, 0xD1}, 2, 0},
  //{0x35, (uint8_t []){0x00}, 1, 0},//TE ON
  {0x53, (uint8_t []){0x20}, 1, 1},
  {0x63, (uint8_t []){0xFF}, 1, 1},
  {0x51, (uint8_t []){0x00}, 1, 1},
  {0x29, (uint8_t []){0x00}, 0, 10},
  {0x51, (uint8_t []){0xFF}, 1, 0},
  //{0x36, (uint8_t []){0x60}, 1, 0},
};

///////////////////////
///////////////////////
static lv_display_t *g_disp = NULL;  // Global display pointer for callback

void lcd_lvgl_Init(void)
{
  READ_LCD_ID = read_lcd_id();

  const spi_bus_config_t buscfg = SH8601_PANEL_BUS_QSPI_CONFIG(EXAMPLE_PIN_NUM_LCD_PCLK,
                                                               EXAMPLE_PIN_NUM_LCD_DATA0,
                                                               EXAMPLE_PIN_NUM_LCD_DATA1,
                                                               EXAMPLE_PIN_NUM_LCD_DATA2,
                                                               EXAMPLE_PIN_NUM_LCD_DATA3,
                                                               EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * LCD_BIT_PER_PIXEL / 8);
  ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));
  esp_lcd_panel_io_handle_t io_handle = NULL;

  const esp_lcd_panel_io_spi_config_t io_config = SH8601_PANEL_IO_QSPI_CONFIG(EXAMPLE_PIN_NUM_LCD_CS,
                                                                              example_notify_lvgl_flush_ready,
                                                                              &g_disp);

  sh8601_vendor_config_t vendor_config = 
  {
    .flags = 
    {
      .use_qspi_interface = 1,
    },
  };
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));
  g_io_handle = io_handle; // Store globally for brightness control
  esp_lcd_panel_handle_t panel_handle = NULL;
  const esp_lcd_panel_dev_config_t panel_config = 
  {
    .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
    .bits_per_pixel = LCD_BIT_PER_PIXEL,
    .vendor_config = &vendor_config,
  };
  vendor_config.init_cmds = (READ_LCD_ID == SH8601_ID) ? sh8601_lcd_init_cmds : co5300_lcd_init_cmds;
  vendor_config.init_cmds_size = (READ_LCD_ID == SH8601_ID) ? sizeof(sh8601_lcd_init_cmds) / sizeof(sh8601_lcd_init_cmds[0]) : sizeof(co5300_lcd_init_cmds) / sizeof(co5300_lcd_init_cmds[0]);
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_lcd_new_panel_sh8601(io_handle, &panel_config, &panel_handle));
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_lcd_panel_init(panel_handle));
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_lcd_panel_disp_on_off(panel_handle, true));

  lv_init();
  lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT * sizeof(lv_color_t), MALLOC_CAP_DMA);
  assert(buf1);

  // Create LVGL display
  lv_display_t *disp = lv_display_create(EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
  g_disp = disp;  // Store globally for callback

  // Set color format to RGB565 (try without byte swapping first)
  // If colors are wrong, try LV_COLOR_FORMAT_RGB565_SWAPPED instead
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

  // Set up single buffering to save DMA memory
  // NOTE: In LVGL 9, buffer size is specified in PIXELS, not bytes
  lv_display_set_buffers(disp, buf1, NULL, EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_BUF_HEIGHT, LV_DISPLAY_RENDER_MODE_PARTIAL);

  // Set the flush callback
  lv_display_set_flush_cb(disp, example_lvgl_flush_cb);

  // Set user data for panel handle
  lv_display_set_user_data(disp, panel_handle);

#ifdef EXAMPLE_Rotate_90
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);
#endif

  // Initialize touch input device
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_display(indev, disp);
  lv_indev_set_read_cb(indev, example_lvgl_touch_cb);

  const esp_timer_create_args_t lvgl_tick_timer_args = 
  {
    .callback = &example_increase_lvgl_tick,
    .name = "lvgl_tick"
  };
  esp_timer_handle_t lvgl_tick_timer = NULL;
  ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

  lvgl_mux = xSemaphoreCreateMutex(); //mutex semaphores
  assert(lvgl_mux);
  xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL);
}

bool example_lvgl_lock(int timeout_ms)
{
  assert(lvgl_mux && "bsp_display_start must be called first");

  const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
  return xSemaphoreTake(lvgl_mux, timeout_ticks) == pdTRUE;
}

void example_lvgl_unlock(void)
{
  assert(lvgl_mux && "bsp_display_start must be called first");
  xSemaphoreGive(lvgl_mux);
}
static void example_lvgl_port_task(void *arg)
{
  uint32_t task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
  for(;;)
  {
    if (example_lvgl_lock(-1))
    {
      task_delay_ms = lv_timer_handler();
      
      example_lvgl_unlock();
    }
    if (task_delay_ms > EXAMPLE_LVGL_TASK_MAX_DELAY_MS)
    {
      task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
    }
    else if (task_delay_ms < EXAMPLE_LVGL_TASK_MIN_DELAY_MS)
    {
      task_delay_ms = EXAMPLE_LVGL_TASK_MIN_DELAY_MS;
    }
    vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
  }
}
static void example_increase_lvgl_tick(void *arg)
{
  lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}
static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
  lv_display_t *disp = (lv_display_t *)user_ctx;
  lv_display_flush_ready(disp);
  return false;
}
static void example_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *color_map)
{
  esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) lv_display_get_user_data(disp);

  // Round area to even boundaries before flushing
  lv_area_t rounded_area = *area;
  rounded_area.x1 &= ~0x1;
  rounded_area.x2 |= 0x1;
  rounded_area.y1 &= ~0x1;
  rounded_area.y2 |= 0x1;

  // Calculate offsets
  const int offsetx1 = (READ_LCD_ID == SH8601_ID) ? rounded_area.x1 : rounded_area.x1 + 0x06;
  const int offsetx2 = (READ_LCD_ID == SH8601_ID) ? rounded_area.x2 : rounded_area.x2 + 0x06;
  const int offsety1 = rounded_area.y1;
  const int offsety2 = rounded_area.y2;

  // The draw_bitmap function takes (x_start, y_start, x_end, y_end)
  // With the rounding above, (offsetx2 + 1) - offsetx1 will always be an even number.
  esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, (lv_color_t *)color_map);

  // Note: lv_display_flush_ready() is called by example_notify_lvgl_flush_ready() callback
  // Do NOT call it here when using DMA with notification callback
}
static void example_lvgl_touch_cb(lv_indev_t *drv, lv_indev_data_t *data)
{
  uint16_t tp_x,tp_y;
  uint8_t win = getTouch(&tp_x,&tp_y);
  if(win)
  {
    data->point.x = tp_x;
    data->point.y = tp_y;
    data->state = LV_INDEV_STATE_PRESSED;
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void lcd_set_brightness(uint8_t brightness)
{
  if (g_io_handle == NULL) {
    return; // Not initialized yet
  }

  // Format command for QSPI interface
  // The display uses QSPI, so we need to encode the command properly
  #define LCD_OPCODE_WRITE_CMD (0x02ULL)
  int lcd_cmd = 0x51; // MIPI DCS command: Write Display Brightness
  lcd_cmd &= 0xff;
  lcd_cmd <<= 8;
  lcd_cmd |= LCD_OPCODE_WRITE_CMD << 24;

  // Send command with brightness value (0x00 = min, 0xFF = max)
  esp_lcd_panel_io_tx_param(g_io_handle, lcd_cmd, &brightness, 1);
}


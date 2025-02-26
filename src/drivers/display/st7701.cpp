#include "st7701.h"

esp_lcd_panel_handle_t panel = NULL;

static spi_device_handle_t spi = NULL;

static void st7701_cmd_write(uint8_t cmd)
{
  spi_transaction_t spi_tran = {
    .cmd = 0,
    .addr = cmd,
    .length = 0,
    .rxlength = 0,
  };
  spi_device_transmit(spi, &spi_tran);
}

static void st7701_data_write(uint8_t data)
{
  spi_transaction_t spi_tran = {
    .cmd = 1,
    .addr = data,
    .length = 0,
    .rxlength = 0,
  };
  spi_device_transmit(spi, &spi_tran);
}

static void st7701_cs_enable()
{
  Set_EXIO(EXIO_PIN3, EXIO_LOW);
  vTaskDelay(pdMS_TO_TICKS(10));
}

static void st7701_cs_disable()
{
  Set_EXIO(EXIO_PIN3, EXIO_HIGH);
  vTaskDelay(pdMS_TO_TICKS(10));
}

static void st7701_reset()
{
  Set_EXIO(EXIO_PIN1, EXIO_LOW);
  vTaskDelay(pdMS_TO_TICKS(10));
  Set_EXIO(EXIO_PIN1, EXIO_HIGH);
  vTaskDelay(pdMS_TO_TICKS(50));
}

// configures the display via SPI - this bus is not used for display data
void st7701_configure()
{
  spi_bus_config_t buscfg = {
    .mosi_io_num = LCD_MOSI_PIN,
    .miso_io_num = -1,
    .sclk_io_num = LCD_CLK_PIN,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 64,
  };
  spi_device_interface_config_t devcfg = {
    .command_bits = 1,
    .address_bits = 8,
    .mode = SPI_MODE0,
    .clock_speed_hz = 40000000,
    .spics_io_num = -1,
    .queue_size = 1, // Not using queues
  };
  spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
  spi_bus_add_device(SPI2_HOST, &devcfg, &spi);

  st7701_cs_enable();
  st7701_cmd_write(0xFF);
  st7701_data_write(0x77);
  st7701_data_write(0x01);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x10);

  st7701_cmd_write(0xC0);
  st7701_data_write(0x3B); // Scan line
  st7701_data_write(0x00);

  st7701_cmd_write(0xC1);
  st7701_data_write(0x0B); // VBP
  st7701_data_write(0x02);

  st7701_cmd_write(0xC2);
  st7701_data_write(0x07);
  st7701_data_write(0x02);

  st7701_cmd_write(0xCC);
  st7701_data_write(0x10);

  st7701_cmd_write(0xCD); // RGB format
  st7701_data_write(0x08);

  st7701_cmd_write(0xB0); // IPS
  st7701_data_write(0x00); // 255
  st7701_data_write(0x11); // 251
  st7701_data_write(0x16); // 247  down
  st7701_data_write(0x0e); // 239
  st7701_data_write(0x11); // 231
  st7701_data_write(0x06); // 203
  st7701_data_write(0x05); // 175
  st7701_data_write(0x09); // 147
  st7701_data_write(0x08); // 108
  st7701_data_write(0x21); // 80
  st7701_data_write(0x06); // 52
  st7701_data_write(0x13); // 24
  st7701_data_write(0x10); // 16
  st7701_data_write(0x29); // 8    down
  st7701_data_write(0x31); // 4
  st7701_data_write(0x18); // 0

  st7701_cmd_write(0xB1); //  IPS
  st7701_data_write(0x00); //  255
  st7701_data_write(0x11); //  251
  st7701_data_write(0x16); //  247   down
  st7701_data_write(0x0e); //  239
  st7701_data_write(0x11); //  231
  st7701_data_write(0x07); //  203
  st7701_data_write(0x05); //  175
  st7701_data_write(0x09); //  147
  st7701_data_write(0x09); //  108
  st7701_data_write(0x21); //  80
  st7701_data_write(0x05); //  52
  st7701_data_write(0x13); //  24
  st7701_data_write(0x11); //  16
  st7701_data_write(0x2a); //  8  down
  st7701_data_write(0x31); //  4
  st7701_data_write(0x18); //  0

  st7701_cmd_write(0xFF);
  st7701_data_write(0x77);
  st7701_data_write(0x01);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x11);

  st7701_cmd_write(0xB0); // VOP  3.5375+ *x 0.0125
  st7701_data_write(0x6d); // 5D

  st7701_cmd_write(0xB1); // VCOM amplitude setting
  st7701_data_write(0x37); //

  st7701_cmd_write(0xB2); // VGH Voltage setting
  st7701_data_write(0x81); // 12V

  st7701_cmd_write(0xB3);
  st7701_data_write(0x80);

  st7701_cmd_write(0xB5); // VGL Voltage setting
  st7701_data_write(0x43); //-8.3V

  st7701_cmd_write(0xB7);
  st7701_data_write(0x85);

  st7701_cmd_write(0xB8);
  st7701_data_write(0x20);

  st7701_cmd_write(0xC1);
  st7701_data_write(0x78);

  st7701_cmd_write(0xC2);
  st7701_data_write(0x78);

  st7701_cmd_write(0xD0);
  st7701_data_write(0x88);

  st7701_cmd_write(0xE0);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x02);

  st7701_cmd_write(0xE1);
  st7701_data_write(0x03);
  st7701_data_write(0xA0);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x04);
  st7701_data_write(0xA0);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x20);
  st7701_data_write(0x20);

  st7701_cmd_write(0xE2);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);

  st7701_cmd_write(0xE3);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x11);
  st7701_data_write(0x00);

  st7701_cmd_write(0xE4);
  st7701_data_write(0x22);
  st7701_data_write(0x00);

  st7701_cmd_write(0xE5);
  st7701_data_write(0x05);
  st7701_data_write(0xEC);
  st7701_data_write(0xA0);
  st7701_data_write(0xA0);
  st7701_data_write(0x07);
  st7701_data_write(0xEE);
  st7701_data_write(0xA0);
  st7701_data_write(0xA0);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);

  st7701_cmd_write(0xE6);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x11);
  st7701_data_write(0x00);

  st7701_cmd_write(0xE7);
  st7701_data_write(0x22);
  st7701_data_write(0x00);

  st7701_cmd_write(0xE8);
  st7701_data_write(0x06);
  st7701_data_write(0xED);
  st7701_data_write(0xA0);
  st7701_data_write(0xA0);
  st7701_data_write(0x08);
  st7701_data_write(0xEF);
  st7701_data_write(0xA0);
  st7701_data_write(0xA0);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);

  st7701_cmd_write(0xEB);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x40);
  st7701_data_write(0x40);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);

  st7701_cmd_write(0xED);
  st7701_data_write(0xFF);
  st7701_data_write(0xFF);
  st7701_data_write(0xFF);
  st7701_data_write(0xBA);
  st7701_data_write(0x0A);
  st7701_data_write(0xBF);
  st7701_data_write(0x45);
  st7701_data_write(0xFF);
  st7701_data_write(0xFF);
  st7701_data_write(0x54);
  st7701_data_write(0xFB);
  st7701_data_write(0xA0);
  st7701_data_write(0xAB);
  st7701_data_write(0xFF);
  st7701_data_write(0xFF);
  st7701_data_write(0xFF);

  st7701_cmd_write(0xEF);
  st7701_data_write(0x10);
  st7701_data_write(0x0D);
  st7701_data_write(0x04);
  st7701_data_write(0x08);
  st7701_data_write(0x3F);
  st7701_data_write(0x1F);

  st7701_cmd_write(0xFF);
  st7701_data_write(0x77);
  st7701_data_write(0x01);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x13);

  st7701_cmd_write(0xEF);
  st7701_data_write(0x08);

  st7701_cmd_write(0xFF);
  st7701_data_write(0x77);
  st7701_data_write(0x01);
  st7701_data_write(0x00);
  st7701_data_write(0x00);
  st7701_data_write(0x00);

  st7701_cmd_write(0x36);
  st7701_data_write(0x00);

  st7701_cmd_write(0x3A);
  st7701_data_write(0x66);

  st7701_cmd_write(0x11);

  vTaskDelay(pdMS_TO_TICKS(480));

  st7701_cmd_write(0x20); //
  vTaskDelay(pdMS_TO_TICKS(120));
  st7701_cmd_write(0x29);
  st7701_cs_disable();

  //  RGB
  esp_lcd_rgb_panel_config_t rgb_config = {
    .clk_src = LCD_CLK_SRC_DEFAULT,
    .timings = {
        .pclk_hz = ESP_PANEL_LCD_RGB_TIMING_FREQ_HZ,
        .h_res = ESP_PANEL_LCD_HEIGHT,
        .v_res = ESP_PANEL_LCD_WIDTH,
        .hsync_pulse_width = ESP_PANEL_LCD_RGB_TIMING_HPW,
        .hsync_back_porch = ESP_PANEL_LCD_RGB_TIMING_HBP,
        .hsync_front_porch = ESP_PANEL_LCD_RGB_TIMING_HFP,
        .vsync_pulse_width = ESP_PANEL_LCD_RGB_TIMING_VPW,
        .vsync_back_porch = ESP_PANEL_LCD_RGB_TIMING_VBP,
        .vsync_front_porch = ESP_PANEL_LCD_RGB_TIMING_VFP,
        .flags = {
            .hsync_idle_low = 0, /*!< The hsync signal is low in IDLE state */
            .vsync_idle_low = 0, /*!< The vsync signal is low in IDLE state */
            .de_idle_high = 0, /*!< The de signal is high in IDLE state */
            .pclk_active_neg = false,
            .pclk_idle_high = 0, /*!< The PCLK stays at high level in IDLE phase */
        },
    },
    .data_width = ESP_PANEL_LCD_RGB_DATA_WIDTH,
    .bits_per_pixel = ESP_PANEL_LCD_RGB_PIXEL_BITS,
    .num_fbs = ESP_PANEL_LCD_RGB_FRAME_BUF_NUM,
    .bounce_buffer_size_px = 10 * ESP_PANEL_LCD_HEIGHT,
    .psram_trans_align = 64,
    .hsync_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_HSYNC,
    .vsync_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_VSYNC,
    .de_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_DE,
    .pclk_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_PCLK,
    .disp_gpio_num = ESP_PANEL_LCD_PIN_NUM_RGB_DISP,
    .data_gpio_nums = {
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA0,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA1,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA2,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA3,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA4,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA5,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA6,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA7,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA8,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA9,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA10,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA11,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA12,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA13,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA14,
        ESP_PANEL_LCD_PIN_NUM_RGB_DATA15,
    },
    .flags = {
        .disp_active_low = 0,
        .refresh_on_demand = 0,
        .fb_in_psram = true,
        .double_fb = true,
        .no_fb = 0,
        .bb_invalidate_cache = 0,
    },
  };
  esp_lcd_new_rgb_panel(&rgb_config, &panel);
  esp_lcd_panel_reset(panel);
  esp_lcd_panel_init(panel);
}

void st7701_setup()
{
  st7701_reset();
  st7701_configure();
  touch_init();
  st7701_backlight_init();
}

void st7701_draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t* data)
{
  x1 = x1 + 1, y1 = y1 + 1;
  if (x1 >= ESP_PANEL_LCD_WIDTH)
    x1 = ESP_PANEL_LCD_WIDTH;
  if (y1 >= ESP_PANEL_LCD_HEIGHT)
    y1 = ESP_PANEL_LCD_HEIGHT;

  esp_lcd_panel_draw_bitmap(panel, x0, y0, x1, y1, data); // x_end End index on x-axis (x_end not included)
}

void st7701_backlight_init()
{
  ledcAttach(LCD_Backlight_PIN, Frequency, Resolution);
  st7701_backlight_set(Backlight_MAX); // 0~100
}

void st7701_backlight_set(uint8_t brightness)
{
  if (brightness > Backlight_MAX || brightness < 0)
    printf("Set Backlight parameters in the range of 0 to 100 \r\n");
  else {
    auto dutyCycle = map(brightness, 0, 100, 0, 1024);
    ledcWrite(LCD_Backlight_PIN, dutyCycle);
  }
}

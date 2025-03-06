#include "st7701.h"

#include <driver/gpio.h>
#include <driver/spi_master.h>

#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>

#include <Arduino.h>

#include "../tca9554pwr/tca9554.h"

#define ST7701_LCD_DATA_WIDTH (16)
#define ST7701_LCD_TIMING_FREQ_HZ (16 * 1000 * 1000)
#define ST7701_LCD_TIMING_HPW (8)
#define ST7701_LCD_TIMING_HBP (10)
#define ST7701_LCD_TIMING_HFP (50)
#define ST7701_LCD_TIMING_VPW (3)
#define ST7701_LCD_TIMING_VBP (8)
#define ST7701_LCD_TIMING_VFP (8)
#define ST7701_BACKLIGHT_PWM_CHANNEL (1)
#define ST7701_BACKLIGHT_PWM_FREQ (20000)
#define ST7701_BACKLIGHT_PWM_RES (10)

static spi_device_handle_t spi = NULL;
static esp_lcd_panel_handle_t panel = NULL;

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

static void st7701_write(uint8_t cmd, const uint8_t* data, size_t len)
{
  st7701_cmd_write(cmd);
  for (size_t i = 0; i < len; i++) {
    st7701_data_write(data[i]);
  }
}

static void st7701_cs_enable()
{
  tca9554pwr_write(PIN_TCA9554PWR_LCD_SPI_CS, LOW);
  vTaskDelay(pdMS_TO_TICKS(10));
}

static void st7701_cs_disable()
{
  tca9554pwr_write(PIN_TCA9554PWR_LCD_SPI_CS, HIGH);
  vTaskDelay(pdMS_TO_TICKS(10));
}

static void st7701_reset()
{
  tca9554pwr_write(PIN_TCA9554PWR_LCD_RESET, LOW);
  vTaskDelay(pdMS_TO_TICKS(10));
  tca9554pwr_write(PIN_TCA9554PWR_LCD_RESET, HIGH);
  vTaskDelay(pdMS_TO_TICKS(50));
}

// configures the display via SPI - this bus is not used for display data
static void st7701_initialize_driver()
{
  spi_bus_config_t buscfg = {
    .mosi_io_num = MOSI,
    .miso_io_num = -1,
    .sclk_io_num = SCK,
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
  {
    // Enter command mode and access page 0x10
    st7701_write(0xFF, (const uint8_t[]) { 0x77, 0x01, 0x00, 0x00, 0x10 }, 5);
    // Set scan line configuration
    st7701_write(0xC0, (const uint8_t[]) { 0x3B, 0x00 }, 2);
    // Set vertical back porch
    st7701_write(0xC1, (const uint8_t[]) { 0x0B, 0x02 }, 2);
    // Set display timing
    st7701_write(0xC2, (const uint8_t[]) { 0x07, 0x02 }, 2);
    // Set display control
    st7701_write(0xCC, (const uint8_t[]) { 0x10 }, 1);
    // Configure RGB interface format
    st7701_write(0xCD, (const uint8_t[]) { 0x08 }, 1);
    // Positive gamma correction for red (IPS)
    st7701_write(0xB0, (const uint8_t[]) { 0x00, 0x11, 0x16, 0x0e, 0x11, 0x06, 0x05, 0x09, 0x08, 0x21, 0x06, 0x13, 0x10, 0x29, 0x31, 0x18 }, 16);
    // Negative gamma correction for red (IPS)
    st7701_write(0xB1, (const uint8_t[]) { 0x00, 0x11, 0x16, 0x0e, 0x11, 0x07, 0x05, 0x09, 0x09, 0x21, 0x05, 0x13, 0x11, 0x2a, 0x31, 0x18 }, 16);
    // Enter command mode and access page 0x11
    st7701_write(0xFF, (const uint8_t[]) { 0x77, 0x01, 0x00, 0x00, 0x11 }, 5);
    // Set VOP voltage (Display driving voltage)
    st7701_write(0xB0, (const uint8_t[]) { 0x6d }, 1);
    // Set VCOM voltage
    st7701_write(0xB1, (const uint8_t[]) { 0x37 }, 1);
    // Set VGH voltage (Gate high voltage)
    st7701_write(0xB2, (const uint8_t[]) { 0x81 }, 1);
    // Power control settings
    st7701_write(0xB3, (const uint8_t[]) { 0x80 }, 1);
    // Set VGL voltage (Gate low voltage)
    st7701_write(0xB5, (const uint8_t[]) { 0x43 }, 1);
    // Additional power settings
    st7701_write(0xB7, (const uint8_t[]) { 0x85 }, 1);
    st7701_write(0xB8, (const uint8_t[]) { 0x20 }, 1);
    st7701_write(0xC1, (const uint8_t[]) { 0x78 }, 1);
    st7701_write(0xC2, (const uint8_t[]) { 0x78 }, 1);
    // Display control
    st7701_write(0xD0, (const uint8_t[]) { 0x88 }, 1);
    // Source timing settings
    st7701_write(0xE0, (const uint8_t[]) { 0x00, 0x00, 0x02 }, 3);
    st7701_write(0xE1, (const uint8_t[]) { 0x03, 0xA0, 0x00, 0x00, 0x04, 0xA0, 0x00, 0x00, 0x00, 0x20, 0x20 }, 11);
    // Source control settings
    st7701_write(0xE2, (const uint8_t[]) { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 13);
    // Timing control settings
    st7701_write(0xE3, (const uint8_t[]) { 0x00, 0x00, 0x11, 0x00 }, 4);
    st7701_write(0xE4, (const uint8_t[]) { 0x22, 0x00 }, 2);
    st7701_write(0xE5, (const uint8_t[]) { 0x05, 0xEC, 0xA0, 0xA0, 0x07, 0xEE, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 16);
    st7701_write(0xE6, (const uint8_t[]) { 0x00, 0x00, 0x11, 0x00 }, 4);
    st7701_write(0xE7, (const uint8_t[]) { 0x22, 0x00 }, 2);
    st7701_write(0xE8, (const uint8_t[]) { 0x06, 0xED, 0xA0, 0xA0, 0x08, 0xEF, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 16);
    // Window settings
    st7701_write(0xEB, (const uint8_t[]) { 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x00 }, 7);
    // Display enhancement settings
    st7701_write(0xED, (const uint8_t[]) { 0xFF, 0xFF, 0xFF, 0xBA, 0x0A, 0xBF, 0x45, 0xFF, 0xFF, 0x54, 0xFB, 0xA0, 0xAB, 0xFF, 0xFF, 0xFF }, 16);
    // Power control settings
    st7701_write(0xEF, (const uint8_t[]) { 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F }, 6);
    // Enter command mode and access page 0x13
    st7701_write(0xFF, (const uint8_t[]) { 0x77, 0x01, 0x00, 0x00, 0x13 }, 5);
    st7701_write(0xEF, (const uint8_t[]) { 0x08 }, 1);
    // Return to page 0
    st7701_write(0xFF, (const uint8_t[]) { 0x77, 0x01, 0x00, 0x00, 0x00 }, 5);
    // Set display orientation
    st7701_write(0x36, (const uint8_t[]) { 0x00 }, 1);
    // Set color mode (18-bit RGB666)
    st7701_write(0x3A, (const uint8_t[]) { 0x66 }, 1);
    // Exit sleep mode
    st7701_write(0x11, NULL, 0);

    vTaskDelay(pdMS_TO_TICKS(480));

    // Display inversion mode off
    st7701_write(0x20, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));

    // Turn on the display
    st7701_write(0x29, NULL, 0);
  }
  st7701_cs_disable();
}

void st7701_initialize_panel()
{
  //  RGB
  esp_lcd_rgb_panel_config_t rgb_config = {
    .clk_src = LCD_CLK_SRC_DEFAULT,
    .timings = {
        .pclk_hz = ST7701_LCD_TIMING_FREQ_HZ,
        .h_res = ST7701_HEIGHT,
        .v_res = ST7701_WIDTH,
        .hsync_pulse_width = ST7701_LCD_TIMING_HPW,
        .hsync_back_porch = ST7701_LCD_TIMING_HBP,
        .hsync_front_porch = ST7701_LCD_TIMING_HFP,
        .vsync_pulse_width = ST7701_LCD_TIMING_VPW,
        .vsync_back_porch = ST7701_LCD_TIMING_VBP,
        .vsync_front_porch = ST7701_LCD_TIMING_VFP,
        .flags = {
            .hsync_idle_low = 0, /*!< The hsync signal is low in IDLE state */
            .vsync_idle_low = 0, /*!< The vsync signal is low in IDLE state */
            .de_idle_high = 0, /*!< The de signal is high in IDLE state */
            .pclk_active_neg = false,
            .pclk_idle_high = 0, /*!< The PCLK stays at high level in IDLE phase */
        },
    },
    .data_width = ST7701_LCD_DATA_WIDTH,
    .bits_per_pixel = ST7701_LCD_PIXEL_BITS,
    .bounce_buffer_size_px = ST7701_WIDTH * 20,
    .sram_trans_align = 32,
    .psram_trans_align = 32,
    .hsync_gpio_num = PIN_LCD_RGB_HSYNC,
    .vsync_gpio_num = PIN_LCD_VSYNC,
    .de_gpio_num = PIN_LCD_DE,
    .pclk_gpio_num = PIN_LCD_PCLK,
    .disp_gpio_num = -1,
    .data_gpio_nums = {
        PIN_LCD_DATA0,
        PIN_LCD_DATA1,
        PIN_LCD_DATA2,
        PIN_LCD_DATA3,
        PIN_LCD_DATA4,
        PIN_LCD_DATA5,
        PIN_LCD_DATA6,
        PIN_LCD_DATA7,
        PIN_LCD_DATA8,
        PIN_LCD_DATA9,
        PIN_LCD_DATA10,
        PIN_LCD_DATA11,
        PIN_LCD_DATA12,
        PIN_LCD_DATA13,
        PIN_LCD_DATA14,
        PIN_LCD_DATA15,
    },
    .flags = {
        .disp_active_low = 0,
        .refresh_on_demand = 0,
        .fb_in_psram = true,
        .double_fb = false,
        .no_fb = 0,
        .bb_invalidate_cache = 0,
    },
  };
  esp_lcd_new_rgb_panel(&rgb_config, &panel);
  esp_lcd_panel_reset(panel);
  esp_lcd_panel_init(panel);
}

void st7701_init()
{
  st7701_reset();
  st7701_initialize_driver();
  st7701_initialize_panel();
  st7701_backlight_init();
}

void st7701_draw_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t* data)
{
  x1 = x1 + 1, y1 = y1 + 1;
  if (x1 >= ST7701_WIDTH)
    x1 = ST7701_WIDTH;
  if (y1 >= ST7701_HEIGHT)
    y1 = ST7701_HEIGHT;

  esp_lcd_panel_draw_bitmap(panel, x0, y0, x1, y1, data);
}

void st7701_backlight_init()
{
  ledcAttach(PIN_LCD_BACKLIGHT, ST7701_BACKLIGHT_PWM_FREQ, ST7701_BACKLIGHT_PWM_RES);
  st7701_backlight_set(ST7701_BACKLIGHT_MAX);
}

void st7701_backlight_set(uint8_t brightness)
{
  if (brightness > ST7701_BACKLIGHT_MAX)
    brightness = ST7701_BACKLIGHT_MAX;
  else if (brightness < 0)
    brightness = 0;

  auto dutyCycle = map(brightness, 0, 100, 0, 1024);
  ledcWrite(PIN_LCD_BACKLIGHT, dutyCycle);
}

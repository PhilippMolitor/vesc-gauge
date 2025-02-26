#include "cst820.h"

static bool cst820_reset(void)
{
  Set_EXIO(EXIO_PIN2, EXIO_LOW);
  vTaskDelay(pdMS_TO_TICKS(10));
  Set_EXIO(EXIO_PIN2, EXIO_HIGH);
  vTaskDelay(pdMS_TO_TICKS(50));

  return true;
}

static bool cst820_config_read(void)
{
  uint8_t buf[3] = { 0 };
  i2c_read(CST820_ADDR, CST820_REG_Version, buf, 1);
  printf("Hardware Version: 0x%02x\n", buf[0]);
  i2c_read(CST820_ADDR, CST820_REG_ChipID, buf, 3);
  printf("ChipID: 0x%02x\nProjID: 0x%02x\nFwVersion: 0x%02x\n", buf[0], buf[1], buf[2]);

  return true;
}

static void cst820_autosleep_set(bool enable)
{
  cst820_reset();
  uint8_t Sleep_State_Set = enable ? 0 : 0xFF;
  i2c_write(CST820_ADDR, CST820_REG_DisAutoSleep, &Sleep_State_Set, 1);
}

uint8_t touch_init(void)
{
  pinMode(CST820_INT_PIN, INPUT_PULLUP);

  cst820_reset();
  cst820_autosleep_set(false);
  cst820_config_read();

  return true;
}

uint8_t touch_read(cst820_event* data)
{
  uint8_t buf[6];
  i2c_read(CST820_ADDR, CST820_REG_GestureID, buf, 6);

  // gesture detected
  if (buf[0] != 0x00)
    data->gesture = (cst820_gesture)buf[0];

  // point touch event
  if (buf[1] != 0x00) {
    noInterrupts();
    data->points = (uint8_t)buf[1];
    if (data->points > CST820_LCD_TOUCH_MAX_POINTS)
      data->points = CST820_LCD_TOUCH_MAX_POINTS;

    data->x = ((buf[2] & 0x0F) << 8) + buf[3];
    data->y = ((buf[4] & 0x0F) << 8) + buf[5];

    interrupts();
  }

  return true;
}

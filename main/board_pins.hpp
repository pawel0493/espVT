#pragma once

#include "driver/gpio.h"
#include "sdkconfig.h"

#include <cstdint>

namespace board
{
#if CONFIG_ISOVT_BOARD_LCD_7
constexpr int LCD_WIDTH = 800;
constexpr int LCD_HEIGHT = 480;

constexpr std::uint32_t LCD_PCLK_HZ = 16'000'000;
constexpr int LCD_HSYNC_PULSE_WIDTH = 4;
constexpr int LCD_HSYNC_BACK_PORCH = 8;
constexpr int LCD_HSYNC_FRONT_PORCH = 8;
constexpr int LCD_VSYNC_PULSE_WIDTH = 4;
constexpr int LCD_VSYNC_BACK_PORCH = 8;
constexpr int LCD_VSYNC_FRONT_PORCH = 8;
constexpr bool LCD_PCLK_ACTIVE_NEG = true;
#else
constexpr int LCD_WIDTH = 1024;
constexpr int LCD_HEIGHT = 600;

constexpr std::uint32_t LCD_PCLK_HZ = 30'000'000;
constexpr int LCD_HSYNC_PULSE_WIDTH = 162;
constexpr int LCD_HSYNC_BACK_PORCH = 152;
constexpr int LCD_HSYNC_FRONT_PORCH = 48;
constexpr int LCD_VSYNC_PULSE_WIDTH = 45;
constexpr int LCD_VSYNC_BACK_PORCH = 13;
constexpr int LCD_VSYNC_FRONT_PORCH = 3;
constexpr bool LCD_PCLK_ACTIVE_NEG = true;
#endif

constexpr bool LCD_SUPPORTS_600_DATA_MASK = LCD_HEIGHT >= 600;

constexpr gpio_num_t I2C_SDA = GPIO_NUM_8;
constexpr gpio_num_t I2C_SCL = GPIO_NUM_9;
constexpr gpio_num_t TOUCH_IRQ = GPIO_NUM_4;
constexpr std::uint8_t TOUCH_I2C_ADDRESS_PRIMARY = 0x5D;
constexpr std::uint8_t TOUCH_I2C_ADDRESS_BACKUP = 0x14;

constexpr gpio_num_t CAN_TX = GPIO_NUM_20;
constexpr gpio_num_t CAN_RX = GPIO_NUM_19;

constexpr int EXIO_TOUCH_RESET = 1;
constexpr int EXIO_LCD_BACKLIGHT = 2;
constexpr int EXIO_LCD_RESET = 3;
constexpr int EXIO_SD_CS = 4;
constexpr int EXIO_CAN_USB_SELECT = 5;
constexpr int EXIO_LCD_VDD_ENABLE = 6;

constexpr bool EXIO_LEVEL_CAN_MODE = true;
constexpr bool EXIO_LEVEL_USB_MODE = false;

constexpr int LCD_HSYNC_GPIO = GPIO_NUM_46;
constexpr int LCD_VSYNC_GPIO = GPIO_NUM_3;
constexpr int LCD_DE_GPIO = GPIO_NUM_5;
constexpr int LCD_PCLK_GPIO = GPIO_NUM_7;
constexpr int LCD_DATA_GPIO[] = {
    GPIO_NUM_14, GPIO_NUM_38, GPIO_NUM_18, GPIO_NUM_17,
    GPIO_NUM_10, GPIO_NUM_39, GPIO_NUM_0, GPIO_NUM_45,
    GPIO_NUM_48, GPIO_NUM_47, GPIO_NUM_21, GPIO_NUM_1,
    GPIO_NUM_2, GPIO_NUM_42, GPIO_NUM_41, GPIO_NUM_40,
};
}

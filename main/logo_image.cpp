#include "logo_image.hpp"

#include "board_pins.hpp"
#include "lcd_panel.hpp"
#include "logo_rgb565.hpp"

namespace logo_image
{
int height()
{
    return logo_asset::HEIGHT;
}

void draw_centered(int top_y)
{
    const int layout_width = board::LCD_HEIGHT < 600 ? 1024 : board::LCD_WIDTH;
    const int left = (layout_width - logo_asset::WIDTH) / 2;
    lcd_panel::draw_rgb565(left, top_y, logo_asset::WIDTH, logo_asset::HEIGHT, logo_asset::PIXELS);
}
}

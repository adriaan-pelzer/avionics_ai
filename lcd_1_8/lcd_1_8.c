#include <string.h>

#include "lcd_1_8.h"
#include "DEV_Config.h"
#include "LCD_1in8.h"
#include "GUI_Paint.h"

bool lcd_1_8_init(lcd_1_8_t *ctx) {
  memset(ctx, 0, sizeof(lcd_1_8_t));
  if (DEV_Module_Init() != 0) {
    return false;
  }
  LCD_1IN8_Init(HORIZONTAL);
  LCD_1IN8_Clear(BLACK);
  LCD_1IN8_SetBacklight(511);
  if ((ctx->image = (uint16_t *) malloc(LCD_1IN8_HEIGHT*LCD_1IN8_WIDTH*2)) == NULL) {
    return false;
  }
  Paint_NewImage((UBYTE *) ctx->image, LCD_1IN8.WIDTH, LCD_1IN8.HEIGHT, 0, BLACK);
  Paint_SetScale(65);
  Paint_Clear(BLACK);
  Paint_SetRotate(ROTATE_0);
  Paint_Clear(BLACK);
  return true;
}

void lcd_1_8_line(lcd_1_8_coord_t *start, lcd_1_8_coord_t *end, uint16_t color, uint8_t width, bool dotted) {
  Paint_DrawLine((UWORD) start->x, (UWORD) start->y, (UWORD) end->x, (UWORD) end->y, (UWORD) color, (DOT_PIXEL) width, (LINE_STYLE) dotted);
}

void lcd_1_8_circle(lcd_1_8_coord_t *centre, uint16_t radius, uint16_t color, uint8_t width, bool filled) {
  Paint_DrawCircle((UWORD) centre->x, (UWORD) centre->y, (UWORD) radius, (UWORD) color, (DOT_PIXEL) width, (DRAW_FILL) filled);
}

void lcd_1_8_update(lcd_1_8_t *ctx) {
  LCD_1IN8_Display((UWORD *) ctx->image);
}

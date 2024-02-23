#ifndef _LCD_1_8_H_
#define _LCD_1_8_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint16_t x;
  uint16_t y;
} lcd_1_8_coord_t;

typedef struct {
  uint16_t *image;
} lcd_1_8_t;

bool lcd_1_8_init(lcd_1_8_t *ctx);
void lcd_1_8_line(lcd_1_8_coord_t *start, lcd_1_8_coord_t *end, uint16_t color, uint8_t width, bool dotted);
void lcd_1_8_circle(lcd_1_8_coord_t *centre, uint16_t radius, uint16_t color, uint8_t width, bool filled);
void lcd_1_8_update(lcd_1_8_t *ctx);

#endif // _LCD_1_8_H_

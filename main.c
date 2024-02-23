#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "bno055.h"
#include "lcd_1_8.h"

static bno055_euler_t euler[2];
static int8_t read_index = 0;

static int8_t calibration_data[22] = {
  0xd5, 0xff, 0x00, 0x00,
  0xe6, 0xff, 0x42, 0x00,
  0xbf, 0x00, 0x1b, 0xff,
  0xfe, 0xff, 0xfd, 0xff,
  0xff, 0xff, 0xe8, 0x03,
  0xc6, 0x02
};
float deg2rad(float deg) {
  return deg / 360.0f * 2.0f * M_PI;
}

void draw_sky(uint16_t radius, lcd_1_8_coord_t *center, uint16_t color) {
  lcd_1_8_circle(center, radius, color, 2, 1);
}

void draw_frame(uint16_t radius, lcd_1_8_coord_t *center, uint16_t color1, uint16_t color2) {
  lcd_1_8_coord_t start, end;
  lcd_1_8_circle(center, radius, color2, 2, 0);
  for (int16_t pitch = -60; pitch <= 60; pitch += 10) { 
    uint8_t dotted = (pitch == 0 ? 1 : 0);
    uint16_t x_len = pitch % 30 == 0 ? 10 : 6;
    if (dotted) {
      x_len = 2 * radius;
    }
    start.x = center->x - x_len / 2;
    start.y = center->y + (int16_t)((float) radius * sinf(deg2rad((float) pitch)));
    end.x = center->x + x_len / 2;
    end.y = start.y;
    lcd_1_8_line(&start, &end, color1, 1, dotted);
  }
  for (int16_t roll = -60; roll <= 60; roll += 15) {
    float roll_rad = deg2rad((float) roll);
    uint16_t len = roll % 30 == 0 ? 6 : 4;
    start.x = center->x + (int16_t)((float) radius * sinf(roll_rad)) * -1;
    start.y = center->y + (int16_t)((float) radius * cosf(roll_rad)) *  1;
    end.x = center->x + (int16_t)((float)(radius - len) * sinf(roll_rad)) * -1;
    end.y = center->y + (int16_t)((float)(radius - len) * cosf(roll_rad)) *  1;
    lcd_1_8_line(&start, &end, color1, 1, 0);
  }
}

void calculate_horizon_coords(uint16_t radius, lcd_1_8_coord_t *center, bno055_euler_t *euler, lcd_1_8_coord_t *start, lcd_1_8_coord_t *end) {
  start->x = center->x + (int16_t)((float) radius * cosf(euler->pitch - euler->roll) * -1.0f);
  start->y = center->y + (int16_t)((float) radius * sinf(euler->pitch - euler->roll) * -1.0f);
  end->x = center->x + (int16_t)((float) radius * cosf(euler->pitch + euler->roll) *  1.0f);
  end->y = center->y + (int16_t)((float) radius * sinf(euler->pitch + euler->roll) * -1.0f);
}

void calculate_marker_coords(uint16_t radius, lcd_1_8_coord_t *center, bno055_euler_t *euler, lcd_1_8_coord_t *start, lcd_1_8_coord_t *end) {
  start->x = center->x + (int16_t)((float) radius * sinf(euler->roll)) *  1;
  start->y = center->y + (int16_t)((float) radius * cosf(euler->roll)) *  1;
  end->x = center->x + (int16_t)((float) (radius - 5) * sinf(euler->roll)) *  1;
  end->y = center->y + (int16_t)((float) (radius - 5) * cosf(euler->roll)) *  1;
}

void core1_entry() {
  bno055_err_t err;
  uint8_t got_calib_data = 0;

  if ((err = bno055_init(calibration_data)) != BNO055_ERR_OK) {
    printf("bno055_init error: %u\n", (uint32_t) err);
    return;
  }

  while (1) {
    int8_t write_index = (read_index + 1) & 0x01;

    bno055_read_euler(&euler[write_index]);

    read_index = write_index;
    sleep_ms(10);
  }
}

void draw_horizon(uint16_t radius, lcd_1_8_coord_t *center, bno055_euler_t *euler, uint16_t color) {
  lcd_1_8_coord_t start, end;
  for (float pitch = deg2rad(90.0f); pitch >= euler->pitch; pitch -= deg2rad(1.0f)) {
    bno055_euler_t e = { .pitch = pitch, .roll = euler->roll };
    calculate_horizon_coords(radius, center, &e, &start, &end);
    lcd_1_8_line(&start, &end, color, 2, 0);
  }
  calculate_marker_coords(radius, center, euler, &start, &end);
  lcd_1_8_line(&start, &end, color, 2, 0);
}

int main () {
  lcd_1_8_t lcd_1_8_ctx;
  lcd_1_8_coord_t center = { .x = 80, .y = 65 };

  stdio_init_all();

  sleep_ms(2000);

  if (!lcd_1_8_init(&lcd_1_8_ctx)) {
    printf("LCD initialisation failed\n");
    return -1;
  }

  multicore_launch_core1(core1_entry);

  while(1) {
    uint16_t radius = 60;
    draw_sky(radius, &center, 0x0c1b);
    draw_horizon(radius, &center, &euler[read_index], 0x5120);
    draw_frame(radius, &center, 0x0000, 0x8430);
    lcd_1_8_update(&lcd_1_8_ctx);
  }
}

#ifndef _BNO055_H_
#define _BNO055_H_

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define BNO055_ADDR 0x28 

typedef enum {
  BNO055_ERR_OK = 0,
  BNO055_ERR_CHIP_ID,
  BNO055_ERR_SELF_TEST_MCU,
  BNO055_ERR_SELF_TEST_GYR,
  BNO055_ERR_SELF_TEST_MAG,
  BNO055_ERR_SELF_TEST_ACC,
  BNO055_ERR_OPR_MODE,
  BNO055_ERR_UNIT_SEL,
  BNO055_ERR_CALIB
} bno055_err_t;

typedef enum {
  CHIP_ID         = 0x00,
#define CHIP_ID_BNO055          0xa0
  EUL_Heading_LSB = 0x1a,
  EUL_Heading_MSG = 0x1b,
  EUL_Roll_LSB    = 0x1c,
  EUL_Roll_MSB    = 0x1d,
  EUL_Pitch_LSB   = 0x1e,
  EUL_Pitch_MSB   = 0x1f,
  QUA_Data_w_LSB  = 0x20,
  QUA_Data_w_MSB  = 0x21,
  QUA_Data_x_LSB  = 0x22,
  QUA_Data_x_MSB  = 0x23,
  QUA_Data_y_LSB  = 0x24,
  QUA_Data_y_MSB  = 0x25,
  QUA_Data_z_LSB  = 0x26,
  QUA_Data_z_MSB  = 0x27,
  TEMP            = 0x34,
  CALIB_STAT      = 0x35,
#define CALIB_STAT_MAG          0x03
#define CALIB_STAT_ACC          0x0c
#define CALIB_STAT_GYR          0x30
#define CALIB_STAT_SYS          0xc0
  ST_RESULT       = 0x36,
#define ST_RESULT_MCU           0x08
#define ST_RESULT_GYR           0x04
#define ST_RESULT_MAG           0x02
#define ST_RESULT_ACC           0x01
  INT_STA         = 0x37,
#define INT_STA_ACC_NOMOTION    0x80
#define INT_STA_ACC_ANYMOTION   0x40
#define INT_STA_ACC_HIGH_G      0x20
#define INT_STA_GYR_HIGH_R      0x08
#define INT_STA_GYR_ANYMOTION   0x04
  SYS_CLK_STATUS  = 0x38,
#define SYS_CLK_STATUS_FREE     0x00
#define SYS_CLK_STATUS_CONF     0x01
  SYS_STATUS      = 0x39,
#define SYS_STATUS_IDLE         0x00
#define SYS_STATUS_ERR          0x01
#define SYS_STATUS_INIT_PERIPH  0x02
#define SYS_STATUS_INIT_SYS     0x03
#define SYS_STATUS_SELF_TEST    0x04
#define SYS_STATUS_FUSE         0x05
#define SYS_STATUS_NOFUSE       0x06
  SYS_ERR         = 0x3a,
#define SYS_ERR_NOERR           0x00
#define SYS_ERR_INIT_PERIPH     0x01
#define SYS_ERR_INIT_SYS        0x02
#define SYS_ERR_SELF_TEST       0x03
#define SYS_ERR_VAL_OOR         0x04
#define SYS_ERR_ADDR_OOR        0x05
#define SYS_ERR_WRITE           0x06
#define SYS_ERR_PWRMODE         0x07
#define SYS_ERR_ACC_PWRMODE     0x08
#define SYS_ERR_FUSION_CONFIG   0x09
#define SYS_ERR_SENSOR_CONFIG   0x0a
  UNIT_SEL        = 0x3b,
#define UNIT_SEL_ORI_MODE       7
#define UNIT_SEL_ORI_MODE_WINDS (0x00 << UNIT_SEL_ORI_MODE)
#define UNIT_SEL_ORI_MODE_ANDRD (0x01 << UNIT_SEL_ORI_MODE)
#define UNIT_SEL_TEMP           4
#define UNIT_SEL_TEMP_CELSIUS   (0x00 << UNIT_SEL_TEMP)
#define UNIT_SEL_TEMP_FAHREN    (0x01 << UNIT_SEL_TEMP)
#define UNIT_SEL_EUL            2
#define UNIT_SEL_EUL_DEG        (0x00 << UNIT_SEL_EUL)
#define UNIT_SEL_EUL_RAD        (0x01 << UNIT_SEL_EUL)
#define UNIT_SEL_GYR            1
#define UNIT_SEL_GYR_DPS        (0x00 << UNIT_SEL_GYR)
#define UNIT_SEL_GYR_RPS        (0x01 << UNIT_SEL_GYR)
#define UNIT_SEL_ACC            0
#define UNIT_SEL_ACC_MPS        (0x00 << UNIT_SEL_ACC)
#define UNIT_SEL_ACC_MG         (0x01 << UNIT_SEL_ACC)
  OPR_MODE        = 0x3d,
#define OPR_MODE_CONFIG         0x00
#define OPR_MODE_ACCONLY        0x01
#define OPR_MODE_MAGONLY        0x02
#define OPR_MODE_GYRONLY        0x03
#define OPR_MODE_ACCMAG         0x04
#define OPR_MODE_ACCGYR         0x05
#define OPR_MODE_MAGGYR         0x06
#define OPR_MODE_ACCMAGGYR      0x07
#define OPR_MODE_IMU            0x08
#define OPR_MODE_COMPASS        0x09
#define OPR_MODE_MAG4GYR        0x0a
#define OPR_MODE_NDOF_FMC_OFF   0x0b
#define OPR_MODE_NDOF           0x0c
  PWR_MODE        = 0x3e,
#define PWR_MODE_NORMAL         0x00
#define PWR_MODE_LOW            0x01
#define PWR_MODE_SUSPEND        0x02
  SYS_TRIGGER     = 0x3f,
#define SYS_TRIGGER_CLK_SEL     0x80
#define SYS_TRIGGER_CLK_SEL_INT 0x00
#define SYS_TRIGGER_CLK_SEL_EXT 0x01
#define SYS_TRIGGER_RST_INT     0x40
#define SYS_TRIGGER_RST_SYS     0x20
#define SYS_TRIGGER_SELF_TEST   0x00
  TEMP_SOURCE     = 0x40,
#define TEMP_SOURCE_ACC         0x00
#define TEMP_SOURCE_GYR         0x01
  AXIS_MAP_CONFIG = 0x41,
#define AXIS_MAP_CONFIG_FROM_X  0x03
#define AXIS_MAP_CONFIG_FROM_Y  0x0c
#define AXIS_MAP_CONFIG_FROM_Z  0x30
#define AXIS_MAP_CONFIG_TO_X    0x00  
#define AXIS_MAP_CONFIG_TO_Y    0x01
#define AXIS_MAP_CONFIG_TO_Z    0x02
  AXIS_MAP_SIGN   = 0x42,
#define AXIS_MAP_SIGN_X         0x04
#define AXIS_MAP_SIGN_Y         0x02
#define AXIS_MAP_SIGN_Z         0x01
#define AXIS_MAP_SIGN_POS       0x00
#define AXIS_MAP_SIGN_NEG       0x01
  ACC_OFFSET_X_LSB  = 0x55,
  ACC_OFFSET_X_MSB  = 0x56,
  ACC_OFFSET_Y_LSB  = 0x57,
  ACC_OFFSET_Y_MSB  = 0x58,
  ACC_OFFSET_Z_LSB  = 0x59,
  ACC_OFFSET_Z_MSB  = 0x5a,
  MAG_OFFSET_X_LSB  = 0x5b,
  MAG_OFFSET_X_MSB  = 0x5c,
  MAG_OFFSET_Y_LSB  = 0x5d,
  MAG_OFFSET_Y_MSB  = 0x5e,
  MAG_OFFSET_Z_LSB  = 0x5f,
  MAG_OFFSET_Z_MSB  = 0x60,
  GYR_OFFSET_X_LSB  = 0x61,
  GYR_OFFSET_X_MSB  = 0x62,
  GYR_OFFSET_Y_LSB  = 0x63,
  GYR_OFFSET_Y_MSB  = 0x64,
  GYR_OFFSET_Z_LSB  = 0x65,
  GYR_OFFSET_Z_MSB  = 0x66,
  ACC_RADIUS_LSB    = 0x67,
  ACC_RADIUS_MSB    = 0x68,
  MAG_RADIUS_LSB    = 0x69,
  MAG_RADIUS_MSB    = 0x6a
} bno055_register_t;

typedef enum {
  CALIB_NONE  = 0,
  CALIB_PART1 = 1,
  CALIB_PART2 = 2,
  CALIB_FULL  = 3
} bno055_calib_value_t;

typedef struct {
  bno055_calib_value_t sys;
  bno055_calib_value_t gyr;
  bno055_calib_value_t acc;
  bno055_calib_value_t mag;
} bno055_calib_stat_t;

typedef struct {
  float yaw;
  float pitch;
  float roll;
} bno055_euler_t;

typedef struct {
  float w;
  float x;
  float y;
  float z;
} bno055_quat_t;

bno055_err_t bno055_init(uint8_t *calibration_data);
void bno055_read_calibration_status(bno055_calib_stat_t *calib_stat);
void bno055_read_calibration_data(uint8_t *calib_data);
void bno055_read_euler(bno055_euler_t *euler);
void bno055_read_quat(bno055_quat_t *quat);

#endif // _BNO055_H_

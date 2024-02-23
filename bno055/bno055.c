#include "bno055.h"

static void i2c_read(uint8_t addr, uint8_t reg, uint8_t *out, uint8_t len) {
  i2c_write_blocking(i2c_default, addr, &reg, 1, true);
  i2c_read_blocking(i2c_default, addr, out, len, false);
}

static uint8_t i2c_write(uint8_t addr, uint8_t reg, uint8_t value) {
  uint8_t buf[2] = { reg, value }, stat;
  i2c_write_blocking(i2c_default, addr, buf, 2, false);
  if (reg == OPR_MODE) {
    if (value == OPR_MODE_CONFIG) {
      sleep_ms(19);
    } else {
      sleep_ms(7);
    }
  }
  i2c_read(addr, SYS_STATUS, &stat, 1);
  return stat;
}

bno055_err_t bno055_init(uint8_t *calibration_data) {
  uint8_t id = 0, retries = 0, stat, st_result;

  i2c_init(i2c_default, 100 * 1000);
  gpio_set_function(0, GPIO_FUNC_I2C);
  gpio_set_function(1, GPIO_FUNC_I2C);
  gpio_pull_up(0);
  gpio_pull_up(1);

  while (retries < 20 && id != CHIP_ID_BNO055) {
    i2c_read(BNO055_ADDR, CHIP_ID, &id, 1);
    sleep_ms(50);
  }

  if (id != CHIP_ID_BNO055) {
    return BNO055_ERR_CHIP_ID;
  }

  i2c_read(BNO055_ADDR, ST_RESULT, &st_result, 1);

  if (st_result & ST_RESULT_MCU == 0) {
    return BNO055_ERR_SELF_TEST_MCU;
  }

  if (st_result & ST_RESULT_GYR == 0) {
    return BNO055_ERR_SELF_TEST_GYR;
  }

  if (st_result & ST_RESULT_MAG == 0) {
    return BNO055_ERR_SELF_TEST_MAG;
  }

  if (st_result & ST_RESULT_ACC == 0) {
    return BNO055_ERR_SELF_TEST_ACC;
  }

  if ((stat = i2c_write(BNO055_ADDR, OPR_MODE, OPR_MODE_CONFIG)) != SYS_STATUS_IDLE) {
    return BNO055_ERR_OPR_MODE;
  }

  if ((stat = i2c_write(BNO055_ADDR, UNIT_SEL, UNIT_SEL_ORI_MODE_WINDS & UNIT_SEL_TEMP_CELSIUS & UNIT_SEL_EUL_RAD)) != SYS_STATUS_IDLE) {
    return BNO055_ERR_UNIT_SEL;
  }

  if (calibration_data) {
    for (bno055_register_t reg = ACC_OFFSET_X_LSB; reg <= MAG_RADIUS_MSB; reg++) {
      if ((stat = i2c_write(BNO055_ADDR, reg, calibration_data[reg - ACC_OFFSET_X_LSB])) != SYS_STATUS_IDLE) {
        return BNO055_ERR_CALIB;
      }
    }
  }

  if ((stat = i2c_write(BNO055_ADDR, OPR_MODE, OPR_MODE_NDOF)) != SYS_STATUS_FUSE) {
    return BNO055_ERR_OPR_MODE;
  }

  return BNO055_ERR_OK;
}

void bno055_read_calibration_status(bno055_calib_stat_t *calib_stat) {
  uint8_t c_stat;
  i2c_read(BNO055_ADDR, CALIB_STAT, &c_stat, 1);
  calib_stat->sys = (bno055_calib_value_t)((c_stat >> 6) & 0x03);
  calib_stat->gyr = (bno055_calib_value_t)((c_stat >> 4) & 0x03);
  calib_stat->acc = (bno055_calib_value_t)((c_stat >> 2) & 0x03);
  calib_stat->mag = (bno055_calib_value_t)((c_stat >> 0) & 0x03);
}

void bno055_read_calibration_data(uint8_t *calib_data) {
  uint8_t x;
  if (i2c_write(BNO055_ADDR, OPR_MODE, OPR_MODE_CONFIG) != SYS_STATUS_IDLE) {
    return;
  }
  i2c_read(BNO055_ADDR, ACC_OFFSET_X_LSB, calib_data, 22);
  if (i2c_write(BNO055_ADDR, OPR_MODE, OPR_MODE_NDOF) != SYS_STATUS_FUSE) {
    return;
  }
}

void bno055_read_euler(bno055_euler_t *euler) {
  uint8_t euler_raw[6];
  i2c_read(BNO055_ADDR, EUL_Heading_LSB, euler_raw, 6);
  euler->yaw   = (float)(int16_t)(euler_raw[2*0] | (((uint16_t) euler_raw[2*0+1]) << 8)) / 900.0f;
  euler->pitch = (float)(int16_t)(euler_raw[2*1] | (((uint16_t) euler_raw[2*1+1]) << 8)) / 900.0f;
  euler->roll  = (float)(int16_t)(euler_raw[2*2] | (((uint16_t) euler_raw[2*2+1]) << 8)) / 900.0f;
}

void bno055_read_quat(bno055_quat_t *quat) {
  uint8_t quat_raw[8];
  i2c_read(BNO055_ADDR, QUA_Data_w_LSB, quat_raw, 8);
  quat->w = (float)(int16_t)(quat_raw[2*0] | (((uint16_t) quat_raw[2*0+1]) << 8)) / (float)(1 << 14);
  quat->x = (float)(int16_t)(quat_raw[2*1] | (((uint16_t) quat_raw[2*1+1]) << 8)) / (float)(1 << 14);
  quat->y = (float)(int16_t)(quat_raw[2*2] | (((uint16_t) quat_raw[2*2+1]) << 8)) / (float)(1 << 14);
  quat->z = (float)(int16_t)(quat_raw[2*3] | (((uint16_t) quat_raw[2*3+1]) << 8)) / (float)(1 << 14);
}

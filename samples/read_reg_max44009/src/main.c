/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/i2c.h>

#define MAX44009_REG_CONFIG		0x02  //register
#define MAX44009_CONTINUOUS_SAMPLING	BIT(7) //value
#define MAX_ADDR 0x4a 
static const struct device *i2c;

static int max44009_reg_read(uint8_t reg, uint8_t *val, bool send_stop)
{
	struct i2c_msg msgs[2] = {
		{
			.buf = &reg,
			.len = 1,
			.flags = I2C_MSG_WRITE,
		},
		{
			.buf = val,
			.len = 1,
			.flags = I2C_MSG_READ,
		},
	};

	if (send_stop) {
		msgs[1].flags |= I2C_MSG_STOP;
	}

	if (i2c_transfer(i2c, msgs, 2, MAX_ADDR) != 0) {
		return -EIO;
	}

	return 0;
}

static int max44009_reg_write( uint8_t reg, uint8_t val)
{
	uint8_t tx_buf[2] = {reg, val};

	return i2c_write(i2c, tx_buf, sizeof(tx_buf),
			 MAX_ADDR);
}


void main(void)
{
        i2c = device_get_binding("I2C_1");
	printk("Hello World! %s\n", CONFIG_BOARD);
	int err = 0;
	uint8_t value = 0U;
	err = max44009_reg_write(MAX44009_REG_CONFIG,0x0);
	if(err!=0){
		printk("Error reading: 0x%x\n", err);
	}else{
		printk("Wrote value 0x%lx\n", 0x0);
	}
	err = max44009_reg_read(MAX44009_REG_CONFIG, &value, true);
	if(err!=0){
		printk("Error reading: 0x%x\n", err);
	}else{
		printk("Read value 0x%x\n", value);
	}
	err = max44009_reg_write(MAX44009_REG_CONFIG,MAX44009_CONTINUOUS_SAMPLING);
	if(err!=0){
		printk("Error reading: 0x%x\n", err);
	}else{
		printk("Wrote value 0x%lx\n", MAX44009_CONTINUOUS_SAMPLING);
	}
	err = max44009_reg_read(MAX44009_REG_CONFIG, &value, true);
	if(err!=0){
		printk("Error reading: 0x%x\n", err);
	}else{
		printk("Read value 0x%x\n", value);
	}
}

/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <nrfx_temp.h>


void temp_handler(int32_t temperature)
{ 
    printk("Temperature (Celcius): %d \n",temperature / 4);
}

void main(void)
{
	int err = 0;

	printk("Temperature sample started\n");

	nrfx_temp_config_t temp_config = NRFX_TEMP_DEFAULT_CONFIG;
        
        err = nrfx_temp_init(&temp_config,temp_handler);
	IRQ_CONNECT(TEMP_IRQn,5,nrfx_temp_irq_handler,NULL,0);
        if (err!=NRFX_SUCCESS) 
	{	
		printk("error %d\n", err);
	}
	while(true){
		err= nrfx_temp_measure();
		k_sleep(K_MSEC(2000));
	}

}

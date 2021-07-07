/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>

#define SLEEP_TIME_MS	1

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,{0});
//static struct gpio_callback button_cb_data;

struct container_type {
	struct gpio_callback button_cb_data;
	const struct gpio_dt_spec *button;
};

static struct container_type container;

/*
 * The led0 devicetree alias is optional. If present, we'll use it
 * to turn on the LED whenever the button is pressed.
 */
static struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios,{0});

void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
	struct container_type *cont = CONTAINER_OF(cb, struct container_type, button_cb_data);
	printk("button device name: %s\n", cont->button->port->name);
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

void main(void)
{
	int ret;
	container.button = &button;//GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,{0});
	if (!device_is_ready(container.button->port)) {
		printk("Error: button device %s is not ready\n",
		       container.button->port->name);
		return;
	}

	ret = gpio_pin_configure_dt(container.button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, container.button->port->name, container.button->pin);
		return;
	}

	ret = gpio_pin_interrupt_configure_dt(container.button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, container.button->port->name, container.button->pin);
		return;
	}

	gpio_init_callback(&container.button_cb_data, button_pressed, BIT(container.button->pin));
	gpio_add_callback(container.button->port, &container.button_cb_data);
	printk("Set up button at %s pin %d\n", container.button->port->name, container.button->pin);

	if (led.port && !device_is_ready(led.port)) {
		printk("Error %d: LED device %s is not ready; ignoring it\n",
		       ret, led.port->name);
		led.port = NULL;
	}
	if (led.port) {
		ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
		if (ret != 0) {
			printk("Error %d: failed to configure LED device %s pin %d\n",
			       ret, led.port->name, led.pin);
			led.port = NULL;
		} else {
			printk("Set up LED at %s pin %d\n", led.port->name, led.pin);
		}
	}

	printk("Press the button\n");
	if (led.port) {
		while (1) {
			/* If we have an LED, match its state to the button's. */
			int val = gpio_pin_get(container.button->port, container.button->pin);

			if (val >= 0) {
				gpio_pin_set(led.port, led.pin, val);
			}
			k_msleep(SLEEP_TIME_MS);
		}
	}
}

/*
 * Copyright (c) 2022 The Chromium OS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/smf.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/shell/shell.h>

#include "controls.h"
#include "meas.h"
#include "view.h"
#include "model.h"

int main(void)
{
	const struct device *dev_pd_analyzer = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

	if (usb_enable(NULL)) {
		return -1;
	}

	meas_init();
	controls_init();
	model_init(dev_pd_analyzer);
	view_init();

	return 0;
}

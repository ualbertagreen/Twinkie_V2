/*
 * Copyright (c) 2022 The Chromium OS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/usb/usb_dc.h>

#include "view.h"
#include "meas.h"
#include "model.h"
#include "mask.h"

static int cmd_meas_cc2(const struct shell *shell, size_t argc, char **argv) {
	int32_t out;

	if (argc == 2 && *argv[1] == 'c') {
		meas_vcon_c(&out);
		shell_print(shell, "current of cc2: %i", out);
	}
	else {
		meas_cc2_v(&out);
		shell_print(shell, "voltage of cc2: %i", out);
	}

	return 0;
}

static int cmd_meas_vb(const struct shell *shell, size_t argc, char **argv) {
	int32_t out;

	if (argc == 2 && *argv[1] == 'c') {
		meas_vbus_c(&out);
		shell_print(shell, "current of vbus: %i", out);
	}
	else {
		meas_vbus_v(&out);
		shell_print(shell, "voltage of vbus: %i", out);
	}

	return 0;
}

static int cmd_meas_cc1(const struct shell *shell, size_t argc, char **argv) {
	int32_t out;

	meas_cc1_v(&out);
	shell_print(shell, "voltage of cc1: %i", out);

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_meas,
        SHELL_CMD(cc1, NULL, "Print cc1 voltage.", cmd_meas_cc1),
        SHELL_CMD(cc2, NULL, "Print cc2 voltage or current.", cmd_meas_cc2),
        SHELL_CMD(vb, NULL, "Print vbus voltage or current.", cmd_meas_vb),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(meas, &sub_meas, "Reads current or voltage of the selected line", NULL);

static int cmd_version(const struct shell *shell, size_t argc, char**argv) {
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(shell, "Twinkie version 2.2.0");

	return 0;
}

SHELL_CMD_REGISTER(version, NULL, "Show Twinkie version", cmd_version);

static int cmd_reset(const struct shell *shell, size_t argc, char**argv) {
	start_snooper(false);
	reset_snooper();
	usb_dc_reset();

	start_snooper(true);
	return 0;
}

SHELL_CMD_REGISTER(reset, NULL, "Resets the Twinkie device", cmd_reset);

static int cmd_snoop(const struct shell *shell, size_t argc, char**argv) {
	if (argc >= 2) {
		switch (*argv[1]) {
		case '0':
			view_set_snoop(0);
			break;
		case '1':
			view_set_snoop(CC1_CHANNEL_BIT);
			break;
		case '2':
			view_set_snoop(CC2_CHANNEL_BIT);
			break;
		case '3':
			view_set_snoop(CC1_CHANNEL_BIT | CC2_CHANNEL_BIT);
		}

	}

	return 0;
}

SHELL_CMD_REGISTER(snoop, NULL, "Sets the snoop CC line, 0 for neither, 3 for both", cmd_snoop);

static int cmd_start(const struct shell *shell, size_t argc, char**argv)
{
	start_snooper(true);
	return 0;
}
SHELL_CMD_REGISTER(start, NULL, "Start snooper", cmd_start);

static int cmd_stop(const struct shell *shell, size_t argc, char**argv)
{
	usb_dc_reset();
	start_snooper(false);
	return 0;
}
SHELL_CMD_REGISTER(stop, NULL, "Stop snooper", cmd_stop);

static int cmd_role(const struct shell *shell, size_t argc, char**argv, void* data) {
	snooper_mask_t pull_mask = (uint32_t) data;

	set_role(pull_mask);

	return 0;
}

SHELL_SUBCMD_DICT_SET_CREATE(role_options, cmd_role,
        (sink, SINK_BIT, "sink"), (source, 0, "source")
);

SHELL_CMD_REGISTER(role, &role_options, "Sets role as sink or source", cmd_role);

static int cmd_cc1_pull(const struct shell *shell, size_t argc, char**argv, void* data) {
	snooper_mask_t pull_mask = (uint32_t) data;

	set_role(pull_mask & CC1_CHANNEL_BIT);

	return 0;
}

static int cmd_cc2_pull(const struct shell *shell, size_t argc, char**argv, void* data) {
	snooper_mask_t pull_mask = (uint32_t) data;

	set_role(pull_mask & CC2_CHANNEL_BIT);

	return 0;
}

SHELL_SUBCMD_DICT_SET_CREATE(cc1_options, cmd_cc1_pull,
        (rd, 0, "resistor disconnected"), (ru, 1, "default resistor"), (r1, 2, "1.5A resistor"), (r3, 3, "3A resistor")
);

SHELL_SUBCMD_DICT_SET_CREATE(cc2_options, cmd_cc2_pull,
        (rd, 0, "resistor disconnected"), (ru, 1, "default resistor"), (r1, 2, "1.5A resistor"), (r3, 3, "3A resistor")
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_rpull,
        SHELL_CMD(cc1, &cc1_options, "Sets pull resistor on cc1", cmd_cc1_pull),
	SHELL_CMD(cc2, &cc2_options, "Sets pull resistor on cc2", cmd_cc2_pull),
        SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(rpull, &sub_rpull, "Place pull resistor", NULL);

static int cmd_output(const struct shell *shell, size_t argc, char**argv, void* data) {
	bool p = (int) data;

	set_empty_print(p);

	return 0;
}

SHELL_SUBCMD_DICT_SET_CREATE(output_options, cmd_output,
        (cont, true, "continuous output"), (pd_only, false, "output only on receiving pd messages")
);

SHELL_CMD_REGISTER(output, &output_options, "Sets console output as continuous or only on receiving PD messages", cmd_output);

static int cmd_auto_stop(const struct shell *shell, size_t argc, char**argv, void* data) {
	bool p = (bool) data;

	set_auto_stop(p);

	return 0;
}

SHELL_SUBCMD_DICT_SET_CREATE(cmd_auto_stop_options, cmd_auto_stop,
        (on, true, "twinkie turns off when no reciever is connected"), (off, false, "twinkie continues printing even when messages are not being received (warning may cause data corruption or desync)")
);

SHELL_CMD_REGISTER(auto_stop, &cmd_auto_stop_options, "Sets to automatically turn off when no valid receiver is connected", cmd_auto_stop);

static int cmd_sleep_time(const struct shell *shell, size_t argc, char**argv, void* data) {
	if (argc == 2) {
		set_sleep_time(atoi(argv[1]));
	}

	return 0;
}

SHELL_CMD_REGISTER(sleep_time, NULL, "Sets console output as continuous or only on receiving PD messages", cmd_sleep_time);

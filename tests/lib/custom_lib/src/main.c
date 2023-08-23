/*
 * Copyright (c) 2021 Legrand North America, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * @file test custom_lib library
 *
 * This suite verifies that the methods provided with the custom_lib
 * library works correctly.
 */

#include <limits.h>

#include <zephyr/ztest.h>

ZTEST(custom_lib, test_get_value)
{

}

ZTEST_SUITE(custom_lib, NULL, NULL, NULL, NULL, NULL);

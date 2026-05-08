// SPDX-License-Identifier: GPL-2.0
#pragma once

#include <linux/printk.h>

enum hb_log_level {
	LOG_LEVEL_DETAIL = 0,
	LOG_LEVEL_ERROR = 1,
};

#define LOG_INFO  "INFO: "
#define LOG_ERROR "ERROR: "

#define hb_printf(level, fmt, ...)                                            \
	do {                                                                  \
		if ((level) == LOG_LEVEL_ERROR)                                \
			pr_err("hyper_box: " fmt, ##__VA_ARGS__);              \
		else                                                          \
			pr_info("hyper_box: " fmt, ##__VA_ARGS__);             \
	} while (0)


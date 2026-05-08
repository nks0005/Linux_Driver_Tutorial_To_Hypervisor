// SPDX-License-Identifier: GPL-2.0
#pragma once

enum hb_error_code {
	ERROR_HW_NOT_SUPPORT = 1,
};

void hb_error_log(enum hb_error_code code);


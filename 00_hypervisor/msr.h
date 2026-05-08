// SPDX-License-Identifier: GPL-2.0
#pragma once

#include <linux/types.h>

#ifdef CONFIG_X86
void hyper_rdmsr(u32 msr, u32 *lo, u32 *hi);
#endif


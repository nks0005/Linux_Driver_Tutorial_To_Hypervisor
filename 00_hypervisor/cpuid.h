// SPDX-License-Identifier: GPL-2.0
#pragma once

#include <linux/types.h>

#ifdef CONFIG_X86
void hyper_cpuid_count(u32 leaf, u32 subleaf, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx);
#endif


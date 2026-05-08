// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/errno.h>
#include <linux/types.h>

#include "cpuid.h"
#include "cpuid_defs.h"
#include "msr.h"
#include "msr_defs.h"
#include "hb_error.h"
#include "hb_log.h"

void hb_error_log(enum hb_error_code code)
{
	hb_printf(LOG_LEVEL_ERROR, LOG_ERROR "hb_error=%d\n", code);
}

static int __init hyper_box_init(void)
{
	u32 eax = 0, ebx = 0, ecx = 0, edx = 0;

	hb_printf(LOG_LEVEL_DETAIL, LOG_INFO "init\n");

#ifdef CONFIG_X86
	hyper_cpuid_count(1, 0, &eax, &ebx, &ecx, &edx);
	hb_printf(LOG_LEVEL_DETAIL,
		  LOG_INFO "cpuid(1,0) eax=%#x ebx=%#x ecx=%#x edx=%#x\n",
		  eax, ebx, ecx, edx);

	if (ecx & CPUID_1_ECX_VMX) {
		hb_printf(LOG_LEVEL_DETAIL, LOG_INFO "    [*] VMX support\n");
	} else {
		hb_printf(LOG_LEVEL_ERROR, LOG_ERROR "    [*] VMX not support\n");
		hb_error_log(ERROR_HW_NOT_SUPPORT);
		return -ENODEV;
	}

	{
		u32 msr_lo = 0, msr_hi = 0;
		u64 feature_ctl;

		hyper_rdmsr(MSR_IA32_FEATURE_CONTROL, &msr_lo, &msr_hi);
		feature_ctl = ((u64)msr_hi << 32) | msr_lo;
		hb_printf(LOG_LEVEL_DETAIL, LOG_INFO "IA32_FEATURE_CONTROL=%#llx\n",
			  feature_ctl);
	}
#else
	hb_printf(LOG_LEVEL_ERROR, LOG_ERROR "cpuid not supported on this arch\n");
	hb_error_log(ERROR_HW_NOT_SUPPORT);
	return -ENODEV;
#endif






	return 0;
}

static void __exit hyper_box_exit(void)
{
	hb_printf(LOG_LEVEL_DETAIL, LOG_INFO "exit\n");
}

module_init(hyper_box_init);    
module_exit(hyper_box_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Linux Driver Tutorial");
MODULE_DESCRIPTION("Hypervisor tutorial module scaffold");
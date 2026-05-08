// SPDX-License-Identifier: GPL-2.0

#ifdef CONFIG_X86

#include <linux/linkage.h>

	.text

#ifdef __x86_64__
/*
 * void hyper_rdmsr(u32 msr, u32 *lo, u32 *hi)
 * SysV AMD64 ABI:
 *   rdi=msr, rsi=lo*, rdx=hi*
 */
SYM_FUNC_START(hyper_rdmsr)
	movl	%edi, %ecx
	rdmsr
	movl	%eax, (%rsi)
	movl	%edx, (%rdx)
	RET
SYM_FUNC_END(hyper_rdmsr)

#else /* i386 */
/*
 * void hyper_rdmsr(u32 msr, u32 *lo, u32 *hi)
 * i386 cdecl:
 *   [esp+4]=msr, [esp+8]=lo*, [esp+12]=hi*
 */
SYM_FUNC_START(hyper_rdmsr)
	movl	4(%esp), %ecx
	rdmsr
	movl	8(%esp), %edi
	movl	%eax, (%edi)
	movl	12(%esp), %edi
	movl	%edx, (%edi)
	RET
SYM_FUNC_END(hyper_rdmsr)
#endif

#endif /* CONFIG_X86 */


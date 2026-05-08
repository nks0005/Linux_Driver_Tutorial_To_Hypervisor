// SPDX-License-Identifier: GPL-2.0

#ifdef CONFIG_X86

#include <linux/linkage.h>

	.text

#ifdef __x86_64__
SYM_FUNC_START(hyper_cpuid_count)
	/* SysV AMD64 ABI:
	 * rdi=leaf, rsi=subleaf, rdx=eax*, rcx=ebx*, r8=ecx*, r9=edx*
	 * rbx is callee-saved, so preserve it.
	 */
	pushq	%rbx
	movq	%rdx, %r10
	movq	%rcx, %r11
	movq	%r8,  %r12
	movq	%r9,  %r13

	movl	%edi, %eax
	movl	%esi, %ecx
	cpuid

	movl	%eax, (%r10)
	movl	%ebx, (%r11)
	movl	%ecx, (%r12)
	movl	%edx, (%r13)

	popq	%rbx
	RET

#else /* i386 */
SYM_FUNC_START(hyper_cpuid_count)
	/* i386 cdecl:
	 * [esp+4]=leaf, [esp+8]=subleaf, [esp+12]=eax*, [esp+16]=ebx*, [esp+20]=ecx*, [esp+24]=edx*
	 * ebx is callee-saved, so preserve it.
	 */
	pushl	%ebx

	movl	8(%esp),  %eax	/* leaf */
	movl	12(%esp), %ecx	/* subleaf */
	cpuid

	movl	16(%esp), %edi	/* eax* */
	movl	%eax, (%edi)
	movl	20(%esp), %edi	/* ebx* */
	movl	%ebx, (%edi)
	movl	24(%esp), %edi	/* ecx* */
	movl	%ecx, (%edi)
	movl	28(%esp), %edi	/* edx* */
	movl	%edx, (%edi)

	popl	%ebx
	RET
#endif

SYM_FUNC_END(hyper_cpuid_count)

#endif /* CONFIG_X86 */


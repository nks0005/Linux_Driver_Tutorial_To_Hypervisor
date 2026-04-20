// SPDX-License-Identifier: GPL-2.0
/*
 * 커널 빌드 옵션(KVM)과 CPU 가상화 관련 기능을 printk 계열로 출력하는 예제 모듈.
 * x86이 아니면 CPUID/VMX 검사는 건너뜁니다.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64)
#include <asm/cpufeature.h>
#include <asm/processor.h>
#endif

static int __init hypervisor_init(void)
{
	pr_info("hypervisor: === virtualization / hypervisor info ===\n");

#if IS_ENABLED(CONFIG_KVM)
	pr_info("hypervisor: 이 커널은 CONFIG_KVM 로 빌드됨 (KVM 코어 포함)\n");
#else
	pr_info("hypervisor: CONFIG_KVM 없음 — 이 커널 빌드에는 KVM 코어가 없음\n");
#endif

#if IS_ENABLED(CONFIG_KVM_INTEL)
	pr_info("hypervisor: CONFIG_KVM_INTEL — Intel KVM(kvm_intel) 빌드 가능\n");
#endif
#if IS_ENABLED(CONFIG_KVM_AMD)
	pr_info("hypervisor: CONFIG_KVM_AMD — AMD KVM(kvm_amd) 빌드 가능\n");
#endif

#if defined(CONFIG_X86_32) || defined(CONFIG_X86_64)
	{
		u32 eax = 1, ebx = 0, ecx = 0, edx = 0;

		native_cpuid(&eax, &ebx, &ecx, &edx);
		if (ecx & (1U << 31))
			pr_info("hypervisor: CPUID leaf1 ECX bit31=1 → 하이퍼바이저 위에서 동작 중일 가능성 큼\n");
		else
			pr_info("hypervisor: CPUID leaf1 ECX bit31=0 → 베어메탈에 가깝거나 해당 비트 미사용\n");

		if (boot_cpu_has(X86_FEATURE_VMX))
			pr_info("hypervisor: CPU — Intel VMX(VT-x) 지원으로 보임\n");
		else
			pr_info("hypervisor: CPU — Intel VMX(VT-x) 미보고 (중첩 가상화/게스트 제한일 수 있음)\n");

		if (boot_cpu_has(X86_FEATURE_SVM))
			pr_info("hypervisor: CPU — AMD SVM(AMD-V) 지원으로 보임\n");
		else
			pr_info("hypervisor: CPU — AMD SVM(AMD-V) 미보고\n");
	}
#else
	pr_info("hypervisor: x86 아님 — VMX/SVM/CPUID leaf1 검사 생략\n");
#endif

	pr_info("hypervisor: 로그 확인: sudo dmesg | tail -30  (또는 journalctl -k)\n");
	return 0;
}

static void __exit hypervisor_exit(void)
{
	pr_info("hypervisor: 모듈 언로드\n");
}

module_init(hypervisor_init);
module_exit(hypervisor_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Print KVM build flags and CPU virtualization hints");
MODULE_AUTHOR("Linux driver tutorial");

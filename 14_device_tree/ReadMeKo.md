# 디바이스 트리(Device Tree)

## x86에서의 하드웨어 탐지(Hardware detection on x86)

- PCI(e), USB 같은 “탐색 가능한(discoverable)” 버스가 존재
- 표준화된 주소가 존재  
  예: 0x03F8 - 0x03FF(첫 번째 시리얼 포트), 0x0CF8 - 0x0CFC(PCI 설정 공간 접근) 등  
  (참고: [osdev wiki](https://wiki.osdev.org/I/O_Ports))
- BIOS 또는 UEFI가 ACPI(Advanced Configuration and Power Interface)를 통해 연결된 하드웨어 정보를 제공

이런 메커니즘은 임베디드 시스템에서는 보통 제공되지 않습니다. 따라서 임베디드 시스템에서는 **디바이스 트리**를 통해 하드웨어를 탐지/설명합니다.

## 디바이스 트리(The Device Tree)

- 디바이스 트리는 Linux 커널에 “사용 가능한 디바이스 정보”를 제공합니다.
- 디바이스 트리는 디바이스 정보를 트리 구조로 요약합니다.
- 디바이스 트리 소스(DTS)와 인클루드(DTSI)는 컴파일되어 DTB(Device Tree Binary)가 됩니다.
- 디바이스는 디바이스 노드(device node)로 표현됩니다.
- 디바이스 트리는 `/sys/firmware/devicetree/base` 및 `/proc/device-tree/` 아래에서 확인할 수 있습니다.
- 사람이 읽기 쉬운 형태로 변환:
  `dtc -I fs -O dts -s /sys/firmware/devicetree/base > /tmp/dt.dts`
- 디바이스 트리는 오버레이(overlay)를 통해 확장할 수도 있습니다.  
  ➞ 디바이스를 추가할 때 매번 전체 디바이스 트리를 다시 컴파일할 필요가 없습니다.

## 디바이스 트리 노드의 중요한 필드(Important fields of a device tree node)

- `compatible`: 디바이스를 드라이버와 연결(match)하는 데 사용
- `status`: 디바이스 활성화("okay") 또는 비활성화("disabled")에 사용
- `reg`: 디바이스의 버스 주소와(선택적으로) 레지스터 크기 정보를 포함
- `interrputs`: 인터럽트 번호 또는 GPIO 핀 정보를 나타냄


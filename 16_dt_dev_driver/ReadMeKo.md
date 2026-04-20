# 16_dt_dev_driver

이번 강의에서는 플랫폼 디바이스(Platform Device)를 예로 들어 Linux 디바이스 드라이버를 구현하는 방법을 배웁니다. 하지만 전반적인 개념은 I2C, PCIe, USB 등 어떤 디바이스에도 동일하게 적용됩니다.

이 예제는 **디바이스 트리(Device Tree)를 지원하는 Linux 시스템에서만** 동작합니다.

## Linux 디바이스 드라이버 작성(Writing a Linux Device Driver)

Linux 디바이스 드라이버를 작성할 때는 보통 다음 단계를 따릅니다.

1. 드라이버가 담당하는 compatible 디바이스를 명시한다.
2. probe 및 remove 함수를 구현한다.
3. probe/remove 함수, compatible 목록, 기타 드라이버 데이터를 드라이버 구조체에 묶는다.
4. OS에 드라이버 구조체를 등록한다.

## compatible 디바이스 이름 지정(Name compatible devices)

compatible 디바이스 목록은 `struct of_device_id` 배열에 넣습니다. 이 목록은 커널이 끝을 인식할 수 있도록 **빈 요소로 종료(terminate)** 되어야 합니다. 그리고 각 compatible 디바이스마다 한 항목씩 추가합니다.

이 예제에서는 드라이버가 지원할 디바이스가 하나뿐이므로, 목록에 한 항목만 추가합니다.

구조체의 `compatible` 필드만 채웁니다. 여기에 넣는 문자열은 디바이스 트리 오버레이에서 사용한 compatible 문자열과 정확히 일치해야 합니다. 이 구조체에는 `data` 필드 등 더 많은 필드가 있으며, 디바이스별 추가 데이터(device specific data)를 전달하는 데 사용할 수 있지만 지금은 필요하지 않습니다.

`MODULE_DEVICE_TABLE` 매크로는 드라이버 자동 로딩을 위한 목록을 유지하는 데 필요합니다.

## probe / remove 함수 구현(Implement probe and remove functions)

다음 단계는 probe 함수와 remove 함수를 구현하는 것입니다.

- probe: compatible 디바이스가 시스템에 추가될 때 호출
- remove: compatible 디바이스가 시스템에서 제거될 때 호출

probe에서는 디바이스에 필요한 리소스를 할당해야 합니다(메모리, 리소스 매핑, 인터럽트 등). remove에서는 이 리소스를 해제합니다. 리소스 할당은 실패할 수 있으므로 probe는 `int` 반환값을 가지며, 성공 시 `0`, 실패 시 음수 에러 코드를 반환합니다. remove는 리소스를 해제만 하므로 반환값이 없습니다.

두 함수 모두 `struct platform_device *` 타입 인자를 하나 받습니다. 이 포인터를 통해 디바이스 트리에 설정된 디바이스 속성(property)에 접근할 수 있습니다.

이 예제의 probe/remove 함수에서는, 함수가 호출되었음을 나타내기 위해 커널 로그에 한 줄을 출력하기만 합니다.

## 드라이버 구조체로 묶기(Packing the driver struct)

compatible 목록, 드라이버 이름, probe/remove 함수를 `struct platform_driver`에 묶습니다.

## 드라이버 등록(Registering the driver)

마지막 단계는 OS에 드라이버를 등록하는 것입니다. 이를 위해 드라이버 init 함수에서 `platform_driver_register`를 호출할 수 있습니다. 인자로 드라이버 구조체 포인터를 넘기며, 성공 시 `0`, 실패 시 음수 에러 코드를 반환합니다.

드라이버를 등록하면, 커널은 compatible 디바이스가 나타날 때마다 드라이버 구조체에 등록된 probe 함수를 호출할 수 있게 됩니다.

드라이버를 해제하려면 exit 함수에서 `platform_driver_unregister`를 호출합니다.

만약 init/exit에서 드라이버 (un)register만 한다면, `module_platform_driver` 매크로를 사용해 init/exit 함수를 자동으로 생성할 수 있습니다. 전역 디바이스 리스트, 디바이스 번호 범위 등 추가 리소스를 init에서 할당해야 하는 경우에만 `platform_driver_register`를 직접 사용하세요.

## 테스트(Testing)

~~~
# 코드 및 DT 오버레이 컴파일
make
# 드라이버 로드
sudo insmod my_dev_driver.ko
# 이제 커널 로그를 확인하면 다음 출력이 보여야 합니다.
# my_dev - Init function is called

# DT 오버레이 삽입
sudo dtoverlay my_overlay.dtbo
# 이제 커널 로그를 확인하면 다음 출력이 보여야 합니다.
# my_dev - Probe Function is called!

# DT 오버레이 제거
sudo dtoverlay -R my_overlay
# 이제 커널 로그를 확인하면 다음 출력이 보여야 합니다.
# my_dev - Remove Function is called!

# 드라이버 언로드
sudo rmmod my_dev_driver
# 이제 커널 로그를 확인하면 다음 출력이 보여야 합니다.
# my_dev - Exit Function is called!
~~~


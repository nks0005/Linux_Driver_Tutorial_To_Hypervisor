# 20_dt_dev_data

이번 강의에서는 디바이스 데이터를 할당하고 사용하는 방법을 배웁니다.

이 예제는 Raspberry Pi에서 컴파일/실행할 수 있습니다. 드라이버 자체는 다른 ARM 기반 플랫폼에서도 동작할 수 있지만, 디바이스 트리 오버레이는 그 플랫폼에 맞게 수정해야 합니다.

## 하드웨어 구성(Hardware setup)

![TODO: Draw Fritzing of HW Setting]()

LED 하나는 GPIO21에, 두 번째 LED는 GPIO16에 연결되어 있습니다.

## 이전 드라이버의 문제점(Problems with the last driver)

이전 강의의 드라이버는 문제가 있습니다. GPIO descriptor를 전역 포인터로 사용하기 때문에, 디바이스 인스턴스를 하나만 처리할 수 있습니다.

만약 compatible 디바이스를 두 개 추가하면 probe 함수가 두 번 실행되고, 전역 포인터가 두 번째 디바이스에 의해 덮어써집니다.

이를 해결하려면, 디바이스마다 별도의 데이터를 할당하고(driver에 바인딩) 사용해야 합니다.

## 디바이스 데이터 정의(Defining device data)

먼저 필요한 디바이스 데이터를 담을 `struct`를 정의합니다. 이 드라이버에서는 디바이스 데이터가 GPIO descriptor 포인터 두 개뿐입니다(LED와 버튼).

~~~
struct my_dev_data {
	struct gpio_desc *led;
	struct gpio_desc *button;
};
~~~

## 디바이스 데이터 할당(Allocating device data)

probe 함수에서 디바이스별 데이터를 할당해야 합니다. `kmalloc`을 사용할 수도 있지만, 그러면 오류 처리나 디바이스 제거 시 직접 메모리를 해제해야 합니다.

다행히 Linux 커널은 디바이스 관리(device-managed) 메모리 할당 함수를 제공합니다.

`devm_kzalloc`을 사용하면, 디바이스에 바인딩된 메모리를 할당하고 0으로 초기화하며, 해당 메모리는 더 이상 필요 없을 때 커널이 자동으로 해제합니다.

- 첫 번째 인자: 메모리를 할당할 대상 `struct device *` 포인터
- 나머지 인자: `kzalloc`과 동일

메모리를 할당한 뒤, 그 데이터 구조체를 사용하면 됩니다.

## 디바이스 데이터 설정(Setting the device data)

디바이스에서 데이터를 사용할 수 있게 하려면 `platform_set_drvdata`를 호출해야 합니다.

- 첫 번째 인자: 데이터를 설정할 플랫폼 디바이스 포인터
- 두 번째 인자: 설정할 데이터 포인터

probe 함수에서는 `platform_get_drvdata`로 데이터를 다시 꺼내 사용할 수 있으며, 이 함수는 디바이스 데이터 포인터를 반환합니다.

## 수정된 오버레이(Modified overlay)

드라이버가 여러 디바이스 인스턴스를 처리할 수 있는지 확인하기 위해, 오버레이에 드라이버와 compatible 한 두 번째 디바이스를 추가했습니다. 드라이버와 오버레이를 로드하면 두 LED가 모두 켜져야 합니다.

## 테스트(Testing)

~~~
# 코드 및 DT 오버레이 컴파일
make
# 드라이버 로드
sudo insmod my_dev_driver.ko

# DT 오버레이 삽입
sudo dtoverlay my_overlay.dtbo

# 이제 두 LED가 모두 켜져 있어야 합니다.

# DT 오버레이 제거
sudo dtoverlay -R my_overlay

# 이제 두 LED가 모두 꺼져 있어야 합니다.

# 드라이버 언로드
sudo rmmod my_dev_driver
~~~


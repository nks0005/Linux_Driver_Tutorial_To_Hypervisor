# 19_dt_dev_gpios

이번 강의에서는 디바이스 트리 기반 디바이스 드라이버에서 GPIO를 사용하는 방법을 배웁니다.

이 예제는 Raspberry Pi에서 컴파일/실행할 수 있습니다. 다른 플랫폼(다른 ARM 보드나 x86)에서는 동작하지 않을 가능성이 큽니다.

## 하드웨어 구성(Hardware setup)

![LED and Button connected to Raspberry Pi](../03_gpioctrl/led_button_Steckplatine.png)

LED는 GPIO21에, 버튼은 GPIO20에 연결되어 있습니다.

## 디바이스 트리 디바이스에 GPIO 추가하기(Adding GPIOs to the device tree device)

먼저 디바이스 트리 오버레이에서 GPIO를 추가해야 합니다. 이를 위해 GPIO가 연결된 GPIO 컨트롤러의 디바이스 트리 노드 이름을 알아야 합니다. 예를 들어 다음과 같이 디바이스 트리를 확인할 수 있습니다.

~~~
cat /proc/device-tree/soc/gpio@7e200000/name
gpio
~~~

따라서 GPIO 컨트롤러 노드 이름은 `gpio`입니다. 이 정보를 바탕으로 LED와 버튼을 다음과 같이 추가할 수 있습니다.

~~~
led-gpio = <&gpio 21 0>;
button-gpio = <&gpio 20 0>;
~~~

GPIO property를 선언할 때는 `-gpio` 접미사를 사용해야 합니다. 이렇게 해야 커널이 GPIO 핀을 감지할 수 있습니다.

- 첫 번째 인자: GPIO 컨트롤러 노드에 대한 포인터
- 두 번째 인자: GPIO 번호
- 마지막 인자: active high 설정(여기서는 0)

디바이스 트리는 하드웨어 추상화를 제공합니다. 3강에서는 드라이버 안에 GPIO 핀 번호를 하드코딩해야 했지만, 여기서는 두 번째 디바이스를 추가하고 GPIO 번호만 바꿔서 재사용할 수 있습니다.

## Linux 디바이스 드라이버에 GPIO 추가하기(Adding the GPIOs to the Linux Device Driver)

드라이버에서는 `struct gpio_desc` 타입의 전역 포인터 두 개를 추가합니다. 이 포인터들이 GPIO 핀을 저장합니다. (전역 변수를 사용하는 방식은 최선이 아니며, 다음 강의에서 더 나은 방법을 보여드립니다.)

probe 함수에서 GPIO property를 읽고 GPIO를 설정해야 합니다. 이를 위해 `gpiod_get` 함수를 사용할 수 있습니다.

- 첫 번째 인자: property를 찾을 디바이스 포인터
- 두 번째 인자: `-gpio` 접미사를 제외한 property 라벨
- 마지막 인자: GPIO를 어떤 방향/초기값으로 설정할지  
  LED는 `GPIOD_OUT_LOW`, 버튼은 `GPIOD_IN`을 사용합니다.

각 호출 이후 유효한 포인터를 받았는지 확인합니다. 이후 `gpiod_set_value`로 LED를 켜거나, `gpiod_get_value`로 버튼 상태를 읽을 수 있습니다.

remove 함수에서는 LED를 끈 뒤, `gpiod_put`으로 GPIO descriptor를 해제합니다.

## 테스트(Testing)

~~~
# 코드 및 DT 오버레이 컴파일
make
# 드라이버 로드
sudo insmod my_dev_driver.ko

# DT 오버레이 삽입
sudo dtoverlay my_overlay.dtbo

# 이제 LED가 켜져 있어야 합니다.
# 커널 로그에서 버튼 상태를 확인할 수 있습니다.

# DT 오버레이 제거
sudo dtoverlay -R my_overlay

# 드라이버 언로드
sudo rmmod my_dev_driver
~~~


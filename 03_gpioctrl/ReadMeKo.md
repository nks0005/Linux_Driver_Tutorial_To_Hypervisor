# 03_gpioctrl

디바이스 트리(Device Tree)를 사용하지 않고, GPIO 핀을 입력/출력으로 접근하는 예제 드라이버입니다.

이 예제는 Raspberry Pi에서 컴파일/실행할 수 있습니다. 다른 플랫폼(다른 ARM 보드나 x86)에서는 동작하지 않을 가능성이 큽니다.

## 하드웨어 구성(Hardware setup)

![LED and Button connected to Raspberry Pi](led_button_Steckplatine.png)

## 올바른 GPIO 번호 찾기

- [pinout.xyz](pinout.xyz)에서 GPIO 번호를 확인합니다.
- GPIO 핀은 gpiochip 단위로 구성됩니다.
- 모든 gpiochip 목록 보기:
  ~~~
  pi@raspberrypi:~ $ gpiodetect 
  gpiochip0 [pinctrl-bcm2835] (54 lines)
  gpiochip1 [raspberrypi-exp-gpio] (8 lines)
  ~~~
- sysfs에서 gpiochip 확인:
  ~~~
  ls /sys/class/gpio/
  export gpiochip0  gpiochip504 unexport
  ~~~
- gpiochip 이름 앞의 숫자는 offset(오프셋)입니다.
- 어떤 gpiochip이 맞는지 확인:
  ~~~
  cat /sys/class/gpiochip0/label
  pinctrl-bcm2835
  ~~~
- 따라서 offset 0을 사용해야 합니다.


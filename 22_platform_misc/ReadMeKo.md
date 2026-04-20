# 22_platform_misc

이번 강의에서는 드라이버가 유저 스페이스(userspace)와 통신할 수 있도록 인터페이스를 추가하는 방법을 배웁니다.

이 예제는 Raspberry Pi에서 컴파일/실행할 수 있습니다. 드라이버 자체는 다른 ARM 기반 플랫폼에서도 동작할 수 있지만, 디바이스 트리 오버레이는 그 플랫폼에 맞게 수정해야 합니다.

## 하드웨어 구성(Hardware setup)

![LED and Button connected to Raspberry Pi](../03_gpioctrl/led_button_Steckplatine.png)

LED는 GPIO21에, 버튼은 GPIO20에 연결되어 있습니다.

## 이전 플랫폼 드라이버의 문제점(Problems with the last platform driver)

이전 강의의 플랫폼 디바이스 드라이버는 GPIO에 접근해 LED를 제어하고 버튼 상태를 읽는 데는 성공합니다. 하지만 문제가 있습니다. 하드웨어 접근이 probe/remove 함수에서만 이루어지고, 유저 스페이스는 드라이버와 통신할 방법이 없습니다. 예를 들어 유저 스페이스에서 LED를 켜거나 끄는 작업을 할 수 없습니다.

따라서 이번에는 드라이버가 유저 스페이스와 통신할 수 있도록 misc 디바이스를 추가해 보겠습니다.

## misc 디바이스 추가하기(Adding the misc device)

시스템에 추가되는 각 compatible 디바이스는 하드웨어를 제어할 수 있는 자신만의 misc 디바이스를 가져야 합니다. 이를 위해 드라이버의 디바이스 데이터 구조체에 `struct miscdevice` 타입 오브젝트를 추가합니다.

probe 함수에서 이 misc 디바이스를 위한 메모리도 할당합니다. 하드웨어 초기화 이후, 파일 연산(file operations), minor 번호, 디바이스 이름을 설정해 misc 디바이스를 준비하고 등록합니다.

각 compatible 디바이스가 각자의 misc 디바이스(=각자의 디바이스 파일)를 가져야 하므로, 디바이스 이름에 번호를 붙여야 합니다. 이를 위해 전역 변수 `devcnt`를 사용해 지금까지 몇 개의 디바이스가 생성되었는지 추적합니다. (예: `my_dev0`) 이름에 번호를 붙인 뒤 `devcnt`를 증가시킵니다.

misc 디바이스 설정이 끝나면 `misc_register`로 등록하며, 이 과정에서 디바이스 파일이 생성됩니다.

remove 함수에서는 misc 디바이스를 deregister하여 디바이스 파일을 제거하고, `devcnt`를 감소시킵니다.

## 콜백 함수 추가하기(Adding function callbacks)

마지막으로, misc 디바이스가 지원할 콜백 함수들을 추가해야 합니다.

- read: 버튼 상태를 읽어오기
- write: LED를 제어하기

하지만 문제가 하나 있습니다. 콜백이 호출되었을 때, “어떤 디바이스 인스턴스의 데이터인지”에 접근해야 합니다. 다행히 misc 디바이스는 `struct file` 포인터의 `private_data` 필드를 설정해 줍니다. 이제 이 포인터로부터 디바이스 데이터를 얻어야 합니다.

이를 위해 `to_my_dev_data` 함수를 추가합니다. 이 함수에 file 포인터를 전달하면, 콜백이 호출된 디바이스의 데이터 포인터를 반환합니다. inline으로 선언해 호출 비용을 줄입니다(예: 함수 호출을 위한 스택 관리 감소).

이 함수는 file 포인터의 `private_data`를 읽는데, 이는 misc 디바이스 포인터입니다. 그리고 `container_of` 매크로를 호출해, 구조체 필드 포인터로부터 원래의 디바이스 데이터 구조체 포인터를 얻습니다.

이 매크로의 동작을 자세히 알고 싶다면 [이 글](https://radek.io/posts/magical-container_of-macro/)을 추천합니다. 요약하면, 구조체의 어떤 필드 포인터를 주면 해당 필드가 포함된 “상위 구조체”의 포인터를 돌려주는 매크로입니다.

read/write 콜백 함수에서는 file 포인터로부터 디바이스 데이터를 얻고, 그 데이터를 통해 LED/버튼에 접근합니다.

## 테스트(Testing)

~~~
# 코드 및 DT 오버레이 컴파일
make
# 드라이버 로드
sudo insmod my_dev_driver.ko

# DT 오버레이 삽입
sudo dtoverlay my_overlay.dtbo

# 다음과 같은 디바이스가 보여야 합니다.
 ls -l /dev/my_dev0 
crw------- 1 root root 10, 121 Feb  4 20:32 /dev/my_dev0

# LED 켜기
echo 1 | sudo tee /dev/my_dev0

# LED 끄기
echo 0 | sudo tee /dev/my_dev0

# 버튼 상태 읽기
sudo cat /dev/my_dev0 
Button is not pressed

# DT 오버레이 제거
sudo dtoverlay -R my_overlay

# 드라이버 언로드
sudo rmmod my_dev_driver
~~~


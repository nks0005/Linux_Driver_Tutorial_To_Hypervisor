# 10_auto_devfile

지금까지는 `mknod`라는 쉘 명령으로 디바이스 파일을 생성했습니다. 이번 강의에서는 Linux 디바이스 매니저인 udev의 도움을 받아, **커널 모듈 내부에서 디바이스 파일을 생성하는 방법**을 보여드립니다.

이 예제는 Raspberry Pi뿐 아니라 일반 x86 컴퓨터에서도 컴파일하고 실행할 수 있습니다.

## 코드 설명(Explaining the code)

이미 우리는 다음을 수행했습니다.

- 디바이스 번호 범위 등록
- 문자 디바이스 생성

여기에 디바이스 파일을 자동 생성하려면 두 단계를 추가로 해야 합니다.

- `/sys/class` 아래에 class 생성
- 해당 class 아래에 device 생성

udev는 우리가 device를 생성한 것을 감지하고, `/dev` 아래에 디바이스 파일을 자동으로 만들어 줍니다.

### 초기화(Initialization)

class는 `struct class *` 타입의 포인터로 표현됩니다. 전역 변수로 `my_class`를 추가합니다. class 생성은 `class_create(const char *class_name)`를 호출해 수행합니다. 인자로 class 이름 문자열을 받고, 성공 시 포인터를 반환하며 실패 시 NULL을 반환합니다. `"my_class"`를 만들면 `/sys/class/my_class` 아래에 나타납니다.

device를 생성하려면 `device_create` 함수를 호출해야 합니다. 인자는 다음과 같습니다.

~~~
struct device *device_create(
    struct class *my_class, /* Class under which the device should be created */
    struct device *parent, /* Parent device */
    void *drvdata, /* Driverdata for the device */
    const char *fmt, ... /* Name of the device */
);
~~~

디바이스 이름은 `printf`처럼 만들 수 있습니다. 예를 들어 `"hello%d", 0`은 `hello0`이라는 이름의 디바이스를 생성합니다. 반환값은 생성된 디바이스 포인터이며, 오류 시 NULL을 반환합니다. 이 예제에서는 parent 디바이스가 없으므로 NULL을 넣지만, PCIe 디바이스 같은 경우 parent로 PCIe 디바이스 포인터를 전달할 수도 있습니다.

`device_create`가 NULL이 아닌 포인터를 반환하면, `/sys/class/my_class/hello0` 아래에 디바이스가 생성된 것입니다. udev는 이 디바이스를 감지해 대응하는 디바이스 파일 `/dev/hello0`를 생성합니다.

### 정리(Cleanup)

정리 단계에서는 디바이스를 제거하고 class를 unregister 및 제거해야 합니다.

~~~
device_destroy(my_class, dev_nr);
class_unregister(my_class);
class_destroy(my_class);
~~~

### 왜 goto를 쓰는가(Why use goto)

`goto`는 C 프로그래밍에서 선호되지 않는 경우가 많습니다. 무분별하게 사용하면 코드가 쉽게 복잡해지기 때문입니다. 하지만 `goto`는 **계층적인 초기화/해제(hierarchical deinitialisation)** 에 매우 적합합니다.

에러가 발생할 때마다 항상 모든 것을 해제하는 대신, 라벨을 계층적으로 만들어 두고 초기화가 어느 단계까지 진행되었는지에 따라 적절한 라벨로 점프하여 그 지점부터 정리하는 방식입니다.

이 방식은 코드를 더 짧게 만들고, 오히려 더 명확하게 만드는 경우가 많습니다. `goto` 없이 init 함수를 구현해 보면, 에러 처리에서 동일한 코드를 반복하게 되어 init 함수가 상당히 길어지는 것을 확인할 수 있을 것입니다.

## 테스트(Testing the code)

`udevadm monitor`를 사용하면 모듈 로드 시 udev의 동작을 모니터링할 수 있습니다. 모듈을 로드한 뒤에는 sysfs의 폴더와 `/dev`의 디바이스 파일이 생성된 것을 확인할 수 있습니다.

~~~
pi@raspberry:~/Programming/Linux_Driver_Tutorial/10_auto_devfile
$ ls /sys/class/my_class/hello0
dev  power  subsystem  uevent
pi@raspberry:~/Programming/Linux_Driver_Tutorial/10_auto_devfile
$ ls /dev/hello0 -lh
crw------- 1 root root 236, 0 Mar 27 10:29 /dev/hello0
~~~

디바이스 파일을 읽으면 커널 로그에 출력이 발생해야 합니다.

모듈을 제거하면 class와 디바이스 파일도 다시 사라집니다.


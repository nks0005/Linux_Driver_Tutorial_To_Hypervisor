# 12_ioctl_simple

우리는 이미 문자 디바이스에서 open/close/read/write 시스템콜을 구현하는 방법을 배웠습니다. 많은 디바이스는 이것만으로도 충분합니다. 하지만 어떤 디바이스는 단순히 읽고 쓰는 것 이상의 기능이 필요합니다. 예를 들어 디바이스 설정을 변경하거나, 설정 값을 읽어오거나, 리셋을 수행하는 기능 등이 그렇습니다. 이런 경우 `ioctl`을 사용해 기능을 제공할 수 있습니다.

## 드라이버에서의 ioctl(ioctl in the driver)

드라이버에 ioctl 기능을 추가하려면 다음 프로토타입의 함수를 만들어야 합니다.

~~~
static long int my_ioctl(struct file *f, unsigned int cmd, unsigned long args)
~~~

- 첫 번째 인자: 현재 열린 파일에 대한 포인터
- `cmd`: 어떤 동작을 수행할지 식별하는 명령 값
- `args`: 명령에 추가로 전달할 인자(포인터)

`ioctl`은 성공 시 0, 실패 시 음수 에러 코드를 반환하는 것이 일반적입니다.

또한 `file_operations` 구조체에서 ioctl 콜백을 설정해, 드라이버가 ioctl을 지원한다는 것을 알려야 합니다.

~~~
static struct file_operations fops = {
	.unlocked_ioctl = my_ioctl,
};
~~~

# 유저 스페이스에서의 ioctl(ioctl in userspace)

간단한 유저 스페이스 프로그램에서는 `ioctl`을 사용하기 위해 `sys/ioctl.h`를 include 해야 합니다. 이후 디바이스 파일을 열고 `ioctl`을 호출하면 됩니다.

첫 번째 인자는 파일 디스크립터, 두 번째는 호출할 명령, 마지막은 전달할 인자 포인터입니다.

반환값을 통해 ioctl 수행이 성공했는지, 오류가 있었는지 확인할 수 있습니다.

## 테스트(Testing the code)

`insmod`로 커널 모듈을 로드한 뒤, 컴파일한 유저 스페이스 프로그램을 실행하세요. ioctl 함수가 트리거되었음을 알려주는 출력이 커널 로그에 나타날 것입니다.


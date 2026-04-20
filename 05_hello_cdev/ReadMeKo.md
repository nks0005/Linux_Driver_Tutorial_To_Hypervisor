# 05_hello_cdev

Linux 드라이버/커널 모듈에서 문자 디바이스(Character Device)를 만드는 방법을 보여주는 예제입니다.

이 예제는 Raspberry Pi뿐 아니라 일반 x86 컴퓨터에서도 컴파일하고 실행할 수 있습니다.

## 코드 설명(Explaining the code)

디바이스 파일을 통해 접근 가능한 인터페이스를 제공하려면, 드라이버가 다음을 수행해야 합니다.

- 디바이스 번호(Device Number)를 할당한다.
- 해당 디바이스 번호와 연결된 문자 디바이스를 만든다.

다행히 Linux에는 이 두 단계를 한 번에 수행하는 함수가 있습니다.

~~~
int register_chrdev(unsigned int major, const char *name, const struct file_operations *fops);
~~~

- `major`: 사용할 Major 디바이스 번호입니다. 0으로 설정하면 `register_chrdev`가 비어 있는 디바이스 번호를 찾아 사용합니다. 이 함수는 동일한 major 번호로 256개의 디바이스 번호(0~255 minor)를 할당합니다.
- `name`: `/proc/devices`에 표시될 라벨 문자열입니다.
- `fops`: 문자 디바이스가 지원할 파일 연산(read/write/open/close/mmap 등) 콜백을 담은 `file_operations` 구조체 포인터입니다.

반환값이 음수면 문자 디바이스 등록 또는 디바이스 번호 할당 과정에서 오류가 발생한 것입니다(예: 이미 사용 중인 번호).

- `major`를 0으로 주면 성공 시 major 번호를 반환합니다.
- `major`에 특정 값을 주면 성공 시 0을 반환합니다.

exit 함수에서는 문자 디바이스를 삭제하고 디바이스 번호를 해제해야 합니다. 이는 다음 함수로 할 수 있습니다.

~~~
void unregister_chrdev(unsigned int major, const char *name);
~~~

## 코드 테스트(Testing the code)

커널 모듈을 로드하면 커널 로그에 다음과 같은 출력이 보일 것입니다.

~~~
[ 3038.299101] hello_cdev - Major Device Number: 236
~~~

Major 디바이스 번호는 시스템마다 다를 수 있습니다. `/proc/devices`를 보면 이 모듈이 사용 중인 번호를 확인할 수 있습니다.

~~~
grep hello_cdev /proc/devices
236 hello_cdev
~~~

이제 해당 디바이스 번호로 디바이스 파일을 만들어 봅시다.

~~~
sudo mknod /dev/hello0 c 236 0
~~~

이 파일을 읽으면 커널 로그에 다음과 같은 출력이 나와야 합니다.

~~~
sudo cat /dev/hello0
sudo dmesg | tail -n 1
[ 3117.149854] hello_cdev - Read is called
~~~

`register_chrdev`가 236부터 시작하는 전체 디바이스 번호 범위를 정말로 할당했는지 확인하기 위해, 두 번째 디바이스 파일을 만들어 봅시다.

~~~
sudo mknod /dev/hello128 c 236 128
~~~

이 파일을 읽어도 커널 로그에 출력이 발생해야 합니다.

~~~
sudo cat /dev/hello128
sudo dmesg | tail -n 1
[ 3120.254910] hello_cdev - Read is called
~~~


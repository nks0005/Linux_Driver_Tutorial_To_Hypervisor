# 06_hello_cdev

문자 디바이스(Character Device)에서 `open`과 `release` 함수를 구현하는 방법을 보여주는 예제입니다.

이 예제는 Raspberry Pi뿐 아니라 일반 x86 컴퓨터에서도 컴파일하고 실행할 수 있습니다.

## 코드 설명(Explaining the code)

`struct file_operations fops`에는 `open`, `read`, `write`, `close` 등 여러 시스템콜에 대한 콜백 함수를 설정할 수 있습니다. 이 예제에서는 문자 디바이스에 대해 다음을 오버라이드했습니다.

- `open`: 유저 스페이스에서 디바이스 파일을 `open`할 때 호출
- `release`: 유저 스페이스에서 `close`할 때 호출

구현하려는 콜백 함수의 프로토타입은 다음과 같습니다.

~~~
int my_open(struct inode *inode, struct file *filp);
int my_release(struct inode *inode, struct file *filp);
~~~

두 함수 모두 성공 시 0, 실패 시 음수 에러 코드를 반환합니다. 인자 또한 동일합니다. 인자 및 중요 필드는 다음과 같습니다.

- `struct inode *inode`: 파일의 커널 표현입니다. 여기서 Major/Minor 디바이스 번호를 얻을 수 있습니다.
- `struct file *filp`: 열려 있는 파일을 나타냅니다. 중요한 필드는 다음과 같습니다.
    - `f_mode`: 파일 권한(읽기/쓰기 등)
    - `f_ops`: 이 파일과 연관된 file operations에 대한 포인터
    - `f_pos`: 파일 내 현재 위치(열릴 때 0)
    - `f_flags`: 유저 스페이스 `open`에서 설정한 플래그(`O_RDONLY`, `O_RDWR` 등)

Major/Minor 디바이스 번호는 `imajor`, `iminor` 함수를 통해 `inode`에서 읽을 수 있습니다.

파일 연산은 `fops` 구조체에 묶여 있습니다.

## 코드 테스트(Testing the code)

드라이버를 테스트하기 위해 간단한 유저 스페이스 프로그램이 필요합니다. 파일 경로를 인자로 넘기면 해당 파일을 여는 프로그램이며, gcc로 컴파일합니다.

~~~
gcc test.c -o cdev_test
~~~

커널 모듈을 로드한 후 디바이스 번호를 확인하고, 디바이스 파일을 몇 개 생성합니다.

~~~
sudo insmod hello_cdev.ko
sudo mknod /dev/hello0 c 236 0
sudo mknod /dev/hello11 c 236 11
~~~

이제 테스트 프로그램을 실행합니다.

~~~
sudo ./cdev_test /dev/hello0
~~~

커널 로그에서 출력이 보일 것입니다. Minor 번호는 0이어야 합니다. `f_mode`와 `f_flags`는 매번 `open` 시 어떤 플래그로 열었는지에 따라 달라집니다.

다른 디바이스 파일로 테스트하면 Minor 번호가 0이 아니라 11로 출력되어야 합니다.

~~~
sudo ./cdev_test /dev/hello11
~~~


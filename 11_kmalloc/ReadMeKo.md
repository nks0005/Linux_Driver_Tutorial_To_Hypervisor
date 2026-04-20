# 11_kmalloc

이번 강의에서는 두 가지 주제를 다룹니다.

- 커널 모듈에서의 동적 메모리 할당
- Linux 커널에서 열린 파일을 나타내는 `struct file`의 `private_data` 포인터

이 예제는 Raspberry Pi뿐 아니라 일반 x86 컴퓨터에서도 컴파일하고 실행할 수 있습니다.

## 동적 메모리 할당(Dynamical memory allocation)

유저 스페이스에서는 `malloc`으로 동적 메모리를 할당하고 `free`로 해제합니다. 커널에서도 비슷한 함수가 제공되며, 커널 함수임을 나타내기 위해 `k` 접두사가 붙습니다. 즉, `kmalloc`과 `kfree`가 있으며 동작 방식은 유저 스페이스의 대응 함수와 유사합니다.

`kmalloc`은 지정한 바이트 수만큼 메모리를 할당하지만 초기화(0으로 채움)는 하지 않습니다. 할당과 동시에 0으로 초기화된 메모리가 필요하면 `kzalloc`을 사용하세요.

`kmalloc`은 두 개의 인자를 받습니다.

- 첫 번째: 할당할 메모리 크기(바이트)
- 두 번째: 커널이 메모리를 어떻게 할당할지에 대한 플래그

아래 표는 몇 가지 예시 플래그입니다.

| flag       | description                                             |
|------------|---------------------------------------------------------|
| GFP_KERNEL | 일반적인 커널 메모리 할당                                |
| GFP_ATOMIC | 인터럽트 컨텍스트에서의 메모리 할당에 사용               |
| GFP_DMA    | DMA 작업을 위한 물리적으로 연속된 메모리 할당            |

물론 메모리를 할당했다면, 더 이상 필요하지 않을 때 반드시 해제하는 것을 잊지 마세요.

## `struct file`의 `private_data`

`struct file`은 Linux 커널에서 열린 파일을 나타냅니다. 이 구조체에는 해당 파일에 종속된 데이터를 저장할 수 있는 `private_data` 포인터가 있습니다.

`struct file` 포인터는 read/write/ioctl 등 모든 파일 연산 콜백에 전달되므로, 그 안의 `private_data`를 통해 파일별 상태나 데이터를 접근/사용할 수 있습니다.

## 코드 설명(Explaining the code)

init 함수에서는 디바이스 번호 범위를 할당하고, 문자 디바이스와 디바이스 파일을 생성합니다. 그리고 문자 디바이스가 사용할 read/write/open/release 함수를 구현합니다.

open 함수에서는 64바이트를 할당하고, 그 포인터를 `struct file`의 `private_data`에 저장합니다. write 콜백에서는 이 메모리에 쓰고, read 콜백에서는 이 메모리에서 읽습니다. release 함수에서는 해당 메모리를 해제합니다.

또한 write 이후에 다시 읽어올 수 있도록, read/write에 전달되는 `loff_t off` 파일 오프셋을 0으로 되돌려야 합니다. 이를 위해 llseek 콜백을 `default_llseek`로 설정합니다.

## 테스트(Testing the code)

모듈을 로드하면 `/dev/hello0` 디바이스 파일이 나타납니다. 이 파일에 대해 읽고 쓰기를 해 봅시다.

~~~
pi@raspberry:~/Programming/Linux_Driver_Tutorial/11_kmalloc
$ echo "Hello World" | sudo tee //dev/hello0
Hello World
pi@raspberry:~/Programming/Linux_Driver_Tutorial/11_kmalloc
$ sudo cat /dev/hello0
ELF���������@8@pi@raspberry:~/Programming/Linux_Driver_Tutorial/11_kmalloc
~~~

왜 "Hello World"를 읽지 못하고 이상한 값(garbage)이 출력될까요? 이유는 동적 할당된 메모리가 **파일이 열려 있는 동안에만** 존재하기 때문입니다. `tee`와 `cat`은 각각 디바이스 파일을 열고, write/read를 수행한 뒤, 바로 파일을 닫습니다. 파일이 닫힐 때 release 콜백에서 메모리를 해제하므로, 다음 프로그램에서 다시 읽을 수 없습니다.

또한 `cat`이 이상한 값을 출력하는 이유는 메모리를 초기화하지 않았기 때문입니다. 만약 `kzalloc`을 사용했다면 0으로 초기화되어 터미널에 아무 것도 보이지 않았을 것입니다.

따라서 유저 스페이스 프로그램에서 디바이스 파일을 열고, 쓰고, `lseek`으로 오프셋을 0으로 되돌린 뒤, 다시 읽는 흐름을 구현하면(예: `test.c`) 같은 open 세션 내에서 방금 쓴 값을 읽어올 수 있습니다. 하지만 파일을 닫는 순간 드라이버에서 메모리를 해제하므로 더 이상 사용할 수 없습니다.


# 09_manual_cdev

지금까지는 `register_chrdev` 함수를 사용해 디바이스 번호 범위를 예약하고, 문자 디바이스를 만들고, 그 범위를 문자 디바이스에 바인딩했습니다. 이번 강의에서는 이 과정을 **수동으로** 수행하는 방법을 배웁니다. 이는 이후 커널 모듈에서 디바이스 파일까지 생성할 때 필요합니다.

이 예제는 Raspberry Pi뿐 아니라 일반 x86 컴퓨터에서도 컴파일하고 실행할 수 있습니다.

## 코드 설명(Explaining the code)

먼저 디바이스 번호(device number)의 형태를 복습해 봅시다. 디바이스 번호는 다음 두 값으로 나뉩니다.

- Major 디바이스 번호
- Minor 디바이스 번호

## 디바이스 번호 범위 예약(Reserving a range of device numbers)

디바이스 번호 범위는 고정된(정적인) 번호로 시작하도록 예약할 수도 있고, 시스템에서 비어 있는 범위를 동적으로 할당받을 수도 있습니다. 두 접근 모두를 살펴봅니다.

#### 고정된(정적인) 디바이스 번호 사용(Using a static device number)

디바이스 번호 `dev_nr`부터 시작해서 `count`개의 디바이스 번호를 예약하려면 다음 함수를 사용할 수 있습니다.

~~~
int register_chrdev_region(dev_t dev_nr, unsigned int count, char *name);
~~~

여기서 `name`은 `/proc/devices`에서 Major 디바이스 번호와 연결된 이름으로 표시됩니다.

#### 동적 디바이스 번호 할당(Using a dynamic device number)

Minor 디바이스 번호 `firstminor`부터 시작하는 `count`개의 범위를, 사용 가능한(비어 있는) 디바이스 번호로 동적으로 할당받으려면 다음 함수를 사용합니다.

~~~
int alloc_chrdev_region(dev_t *dev_nr, unsigned int firstminor,  unsigned int count, char *name);
~~~

첫 번째 인자는 할당된 디바이스 번호를 저장할 포인터입니다.

두 함수 모두 성공 시 0, 실패 시 에러 코드를 반환합니다.

## 문자 디바이스 생성(Creating the character device)

문자 디바이스는 `struct cdev` 타입의 오브젝트로 표현됩니다. 이를 사용하려면 `linux/cdev.h`를 include 해야 합니다. 이 커널 모듈에서는 해당 타입의 전역 변수를 선언합니다.

~~~
static struct cdev my_cdev;
~~~

이 오브젝트는 init 함수에서 초기화합니다. 이를 위해 `cdev_init` 함수를 사용할 수 있고, 소유자(owner)를 현재 모듈로 설정합니다.

~~~
cdev_init(&my_cdev, &fops);
my_cdev.owner = THIS_MODULE;
~~~

`cdev_init`의 첫 번째 인자는 cdev 포인터이고, 두 번째 인자는 이 cdev가 지원할 파일 연산 콜백을 담은 `file_operations` 구조체 포인터입니다. 이 함수는 반환값이 없습니다.

문자 디바이스를 시스템에서 사용 가능하게 하려면 `cdev_add`로 등록해야 합니다.

~~~
int cdev_add(struct cdev *my_cdev, dev_t dev_nr, unsigned int count);
~~~

- 첫 번째 인자: cdev 포인터
- 두 번째 인자: 바인딩할 디바이스 번호
- 세 번째 인자: 이 cdev가 관리하는 디바이스 번호 개수

성공 시 0, 실패 시 음수 에러 코드를 반환합니다.

## 정리(Cleanup)

정리 단계에서는 cdev를 삭제하고, 디바이스 번호 범위를 해제해야 합니다. 다음 함수들을 사용합니다.

~~~
void cdev_del(struct cdev *my_cdev);
void unregister_chrdev_region(dev_t from, unsigned int count);
~~~

## 테스트(Testing the code)

이 모듈은 [05_hello_cdev](../05_hello_cdev/README.md) 예제와 동일하게 동작해야 합니다.


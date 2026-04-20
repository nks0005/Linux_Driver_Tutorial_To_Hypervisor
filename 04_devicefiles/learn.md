# `/dev/???`는 파일 디스크립터(fd)가 아니라 `디바이스 노드(특수 파일)`이다.

> `/dev/hello` > 커널로 들어가는 `문(입구)` 같은 특수 파일
> 이 디바이스 노드에는 `타입 c/b`, `major`, `minor` 정보가 들어있고, 커널은 이 번호로 어느 드라이버로 연결할지 결정한다.
> 유저 프로세스는 major/minor를 직접 다루지 않고, `open("/dev/xxx")`로 fd를 얻어서 `read/write/ioctl`을 호출한다.


## 1. `/dev` v.s. `fd` 
> `/dev/hello` : 커널로 들어가는 `문(입구)` 같은 특수 파일
> `fd` : 유저 프로세스가 `open("/dev/hello")`를 했을 때 프로세스 내부에 생기는 핸들(정수)

## 2. `Major/Minor`의 의미와 위치
> `ls -l /dev/hello` 에서
    > 앞 글자 `c/b`는 문자/블록 디바이스 구분
    > 파일 크기 자리 대신 `major/minor`가 출력됨
        crw-------   1 root     root     10, 263 Apr 20 09:21 hello

## 3. `/dev/xxx`가 자동 생성되려면 드라이버가
> 문자 디바이스를 커널에 등록(major/minor 확보 + fops 연결)하고
> udev가 `/dev/`를 만들 수 있도록 `디바이스 생성 이벤트/정보를 제공`해야 한다.

## 4. hello.c
> `miscdevice 방식`
> 동작 흐름:
    > 1. `insmod hello.ko`
    > 2. 커널이 `my_init()` 실행
    > 3. `misc_register(&hello_miscdev)`
    > 4. 커널 등록 + uevent
    > 5. udev가 `/dev/hello` 생성

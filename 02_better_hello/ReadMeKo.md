# 02_better_hello

개선된 Hello World Linux 커널 모듈 예제입니다.

이 예제는 Raspberry Pi뿐 아니라 일반 x86 컴퓨터에서도 컴파일하고 실행할 수 있습니다.

## 변경 사항(Changes)

다음과 같은 개선이 추가되었습니다.

- 커널 모듈에 대한 메타데이터 추가
- `my_init`, `my_exit` 함수를 `static`으로 선언하여 가시성과 링키지를 제한
- 가독성을 높이기 위해 `__init`, `__exit` 매크로 추가

## 커널 모듈 관리 명령어

- 커널 모듈 메타데이터 보기:
  ~~~
  # 경로/파일명으로 지정
  modinfo ./hello.ko
  # 커널에 포함되어 배포되는 모듈의 경우
  modinfo industrialio
  ~~~
- 로드된 모듈 목록 보기:
  ~~~
  lsmod
  # 목록에서 hello만 찾기
  lsmod | grep hello
  ~~~
- 커널 로그 보기:
  ~~~
  # 전체 커널 로그
  sudo dmesg
  # 로그 마지막 5줄만 보기
  sudo dmesg | tail -n 5
  # 커널 로그를 출력하고 새 로그를 계속 따라가기
  sudo dmesg -w
  ~~~
- 의존성을 포함해 모듈 로드(설치된 커널과 함께 제공되는 모듈에만 해당):
  ~~~
  sudo modprobe industrialio
  ~~~
- 모듈 제거:
  ~~~
  # 경로/파일명으로 지정
  sudo rmmod ./hello.ko
  # lsmod에 표시되는 모듈 이름으로 제거
  sudo rmmod hello
  ~~~


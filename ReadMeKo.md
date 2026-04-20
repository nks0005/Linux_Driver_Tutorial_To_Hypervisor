# Linux 드라이버 튜토리얼

이 저장소에는 간단한 Linux 커널 모듈(LKM)과 Linux 드라이버 예제들이 들어 있습니다.

## 준비(Preparation)

저는 Raspberry Pi 3에서 모듈/드라이버를 개발하고 테스트했습니다. 컴파일하려면 Pi에 커널 헤더를 설치해야 합니다. Raspberry Pi OS에서는 아래 명령으로 설치할 수 있습니다.

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install raspberrypi-kernel-headers
reboot
```

Raspberry Pi OS는 **최신 커널 헤더만** 설치합니다. 따라서 **최신 커널을 실행 중인지** 확인하세요.

또한 빌드 유틸리티(make, gcc 등)가 필요하지만, Raspberry Pi OS에는 보통 기본으로 설치되어 있습니다.

## 구성(Content)

이 저장소에서 다루는 예제는 다음과 같습니다.

1. 간단한 Hello World 커널 모듈
2. 개선된 Hello World 커널 모듈
3. 디바이스 트리 없이 드라이버에서 GPIO를 사용하는 예제
4. 디바이스 파일, 디바이스 번호, 블록/문자 디바이스 소개
5. 드라이버에서 문자 디바이스(Character Device) 생성
6. Linux 커널 로그 레벨
7. 문자 디바이스의 open/release 함수 구현
8. 문자 디바이스의 read/write 함수 구현
9. 문자 디바이스를 수동으로 생성하기
10. 드라이버 내부에서 디바이스 파일을 자동으로 생성하기
11. 동적 메모리 할당 및 `struct file`의 `private_data`
12. `ioctl` 첫 소개
13. `ioctl`을 올바르게 구현하기
14. 디바이스 트리(Device Tree)
15. 디바이스 트리를 통해 디바이스 추가하기
16. 디바이스 트리 기반 디바이스 드라이버
17. 디바이스 트리 드라이버의 파라미터 파싱
18. 디바이스 정보를 포함해서 커널 로그에 기록하기
19. 디바이스 트리 드라이버에서 GPIO 사용하기
20. 디바이스 데이터 할당 및 사용하기
21. 간단한 디바이스 파일 생성을 위한 misc 디바이스
22. 플랫폼 디바이스 드라이버에 misc 디바이스 추가하기

## 추가 정보(More Information)

더 자세한 내용은 제가 만든 [영상 및 재생목록](https://www.youtube.com/watch?v=x1Y203vH-Dc&list=PLCGpd0Do5-I3b5TtyqeF1UdyD4C-S-dMa)을 참고하세요.

## 후원(Support me)

후원하고 싶다면 [buymeacoffee.com/johannes4linux](https://www.buymeacoffee.com/johannes4linux)에서 커피 한 잔을 사주실 수 있습니다.


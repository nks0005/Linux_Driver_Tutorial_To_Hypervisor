# Miscellaneous Devices

misc 디바이스(miscdev)를 사용해, Linux 드라이버/커널 모듈에서 **쉽게 디바이스 파일을 생성**하는 방법을 보여주는 예제입니다.

이 예제는 Raspberry Pi뿐 아니라 일반 x86 컴퓨터에서도 컴파일하고 실행할 수 있습니다.

## 왜 사용해야 하나요?(Why should I use it?)

Miscellaneous 디바이스 API는 디바이스 파일을 생성하는 아주 쉬운 방법을 제공합니다. 전통적인(일반적인) 문자 디바이스 방식에 비해 필요한 단계가 더 적습니다. 따라서 간단한 디바이스를 만들고 디바이스 파일이 필요하다면, misc 디바이스가 도움이 될 수 있습니다.

## 코드 설명(Explaining the code)

이 예제 코드는 `my_miscdev`라는 이름의 misc 디바이스를 만들고, 드라이버 안에 정의된 전역 문자열을 읽고 쓰기 위한 read/write 콜백을 구현합니다.

먼저 read/write 콜백 함수를 구현하고, 이를 `struct file_operations`로 묶습니다.

그 다음 misc 디바이스 `my_miscdev`를 생성하고 다음 필드를 설정합니다.

- `.fops`: 지원할 파일 연산(file operations)에 대한 포인터로 초기화
- `.name`: misc 디바이스 이름이며, 디바이스 파일 이름으로도 사용됨
- `.minor`: misc 디바이스의 minor 번호 설정  
  고정 값(예: 12)을 줄 수도 있고, `MISC_DYNAMIC_MINOR` 매크로로 시스템이 비어 있는 minor 번호를 자동 선택하게 할 수도 있습니다.

init 함수에서 `misc_register`로 misc 디바이스를 등록합니다. 이 함수는 miscdev 포인터를 인자로 받고, 성공 시 0, 실패 시 음수 에러 코드를 반환합니다.

`my_miscdev.minor` 필드를 통해 할당된 minor 번호를 확인할 수 있습니다.

exit 함수에서는 `misc_deregister`로 misc 디바이스를 해제합니다(이 함수도 miscdev 포인터를 인자로 받습니다).

## 테스트(Testing the code)

커널 모듈을 로드하면 커널 로그에 다음과 같은 출력이 보일 것입니다.

~~~
[ 3038.299101] hello_miscdev - /dev/my_miscdev created with minor dev nr: 121
~~~

즉, `my_miscdev`가 minor 번호 121로 생성된 것입니다. 이 값은 동적으로 할당되므로 시스템마다 다를 수 있습니다.

`/dev`에서 디바이스 파일을 확인할 수 있어야 합니다.

~~~
ls -l /dev/my_miscdev 
crw------- 1 root root 10, 121 Jan 27 20:27 /dev/my_miscdev
~~~

모든 misc 디바이스는 major 디바이스 번호 10을 공유합니다. 이는 일반 문자 디바이스와의 차이점 중 하나입니다. 일반 문자 디바이스는 major/minor를 직접 할당할 수 있지만, misc 디바이스는 major 10을 고정으로 공유하면서 `(1 << 20) - 1`개의 가능한 minor 번호를 함께 사용해야 합니다.

이제 디바이스 파일에 쓰고 읽어 보며 테스트합니다.

~~~
echo "Hello World!" | sudo tee /dev/my_miscdev
~~~

쓰기 이후 write 콜백이 한 번 실행되는 것을 확인할 수 있습니다. 읽어오려면 다음을 실행합니다.

~~~
sudo cat /dev/my_miscdev
Hello World!
~~~

읽기/쓰기가 정상 동작하는 것을 확인할 수 있습니다.

## 요약(Summary)

misc 디바이스는 드라이버 내부에서 디바이스 파일을 쉽게 만들 수 있는 방법입니다. 코드도 더 짧고 단계도 적지만, major 번호가 고정(10)이라는 제약이 있습니다. 하지만 간단한 디바이스라면 이 제약은 보통 문제가 되지 않습니다.

## 참고 자료(Resources)

[LinuxVox article](https://linuxvox.com/blog/what-is-the-difference-between-misc-drivers-and-char-drivers/#what-are-miscellaneous-misc-drivers)


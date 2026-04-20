# 18_dev_logging

이번 강의에서는 Linux 커널 로그에 기록할 때 디바이스 정보를 함께 포함시키는 방법을 배웁니다.

이 예제는 **디바이스 트리(Device Tree)를 지원하는 Linux 시스템에서만** 동작합니다.

## 새로운 로깅 함수(New logging functions)

지금까지는 `printk`에 로그 레벨을 부여하는 매크로(예: `pr_info`)를 사용했습니다. 하지만 이제는 제대로 된 Linux 디바이스 드라이버가 있으니, 커널 로그에 디바이스 관련 정보를 더 포함하면 좋습니다. 이를 위해 다음과 같은 “디바이스 로깅” 매크로들이 제공됩니다.

|     Name     | String |                 Alias function                  |
|:------------:|:------:|:-----------------------------------------------:|
| KERN_EMERG   | “0”    | dev_emerg()                                     |
| KERN_ALERT   | “1”    | dev_alert()                                     |
| KERN_CRIT    | “2”    | dev_crit()                                      |
| KERN_ERR     | “3”    | dev_err()                                       |
| KERN_WARNING | “4”    | dev_warn()                                      |
| KERN_NOTICE  | “5”    | dev_notice()                                    |
| KERN_INFO    | “6”    | dev_info()                                      |
| KERN_DEBUG   | “7”    | dev_debug() and dev_devel() if DEBUG is defined |
| KERN_DEFAULT | “”     |                                                 |

이 매크로들은 포맷 문자열과 인자 외에도, “어떤 디바이스에 대한 로그인지”를 알 수 있도록 디바이스 포인터가 필요합니다. 따라서 `struct device *` 타입 포인터를 매크로에 전달해야 합니다. 예를 들어 플랫폼 디바이스의 경우:

~~~
dev_info(&pdev->dev, "Remove Function is called!\n");
~~~

## 디바이스 트리 오버레이(Device Tree overlay)

조금 더 흥미롭게 만들기 위해, 디바이스 트리 오버레이에는 두 개의 디바이스가 정의되어 있습니다. 두 디바이스 모두 `my_dev_driver`와 compatible 합니다.

- 첫 번째 디바이스 이름: `my_device`
- 두 번째 디바이스 이름: `my_dev2`  
  `my_dev2`에는 주소(0x12345678)도 할당되어 있습니다.

## 테스트(Testing)

~~~
# 코드 및 DT 오버레이 컴파일
make
# 드라이버 로드
sudo insmod my_dev_driver.ko

# DT 오버레이 삽입
sudo dtoverlay my_overlay.dtbo
# 이제 커널 로그를 확인하면 출력이 보일 것입니다.

# DT 오버레이 제거
sudo dtoverlay -R my_overlay
[ 1372.534913] my_dev_driver my_device: Probe Function is called!
[ 1372.534946] my_dev_driver my_device: This is a warning
[ 1372.534959] my_dev_driver my_device: This is an error: 123
[ 1372.539672] my_dev_driver my_dev2@12345678: Probe Function is called!
[ 1372.539706] my_dev_driver my_dev2@12345678: This is a warning
[ 1372.539719] my_dev_driver my_dev2@12345678: This is an error: 123


# 드라이버 언로드
sudo rmmod my_dev_driver
[ 1410.134981] my_dev_driver my_dev2@12345678: Remove Function is called!
[ 1410.135248] my_dev_driver my_device: Remove Function is called!
~~~

커널 로그 라인 앞에 드라이버 이름과 디바이스 이름이 추가되는 것을 볼 수 있습니다. 디바이스에 주소가 있다면 주소도 함께 표시됩니다.


# 06_kernel_logging

이 예제에서는 `printk`에서 서로 다른 로그 레벨을 사용하는 방법과, 커널 로그에서 그 레벨이 어떻게 표시되는지(포맷) 확인할 수 있습니다.

이 예제는 Raspberry Pi뿐 아니라 일반 x86 컴퓨터에서도 컴파일하고 실행할 수 있습니다.

관련 내용은 [Linux Kernel 문서](https://www.kernel.org/doc/html/latest/core-api/printk-basics.html)에서도 확인할 수 있습니다.

## printk 함수

`printk`는 보통 다음처럼 사용합니다.

~~~
printk(KERN_INFO "Message: %s\n", arg);
~~~

여기서 `KERN_INFO`는 해당 커널 로그 라인의 로그 레벨을 의미합니다.

## 커널 로그 레벨(Kernel Log Levels)

사용 가능한 로그 레벨은 다음과 같습니다.

|     Name     | String |                 Alias function                |
|:------------:|:------:|:---------------------------------------------:|
| KERN_EMERG   | “0”    | pr_emerg()                                    |
| KERN_ALERT   | “1”    | pr_alert()                                    |
| KERN_CRIT    | “2”    | pr_crit()                                     |
| KERN_ERR     | “3”    | pr_err()                                      |
| KERN_WARNING | “4”    | pr_warn()                                     |
| KERN_NOTICE  | “5”    | pr_notice()                                   |
| KERN_INFO    | “6”    | pr_info()                                     |
| KERN_DEBUG   | “7”    | pr_debug() and pr_devel() if DEBUG is defined |
| KERN_DEFAULT | “”     |                                               |
| KERN_CONT    | “c”    | pr_cont()                                     |

## 로그 레벨로 필터링하기(Filtering for log levels)

`dmesg`의 `-l` 옵션을 사용하면 특정 로그 레벨만 필터링할 수 있습니다.

~~~
# Debug 메시지만 보기
sudo dmesg -l 7

# Critical 메시지만 보기
sudo dmesg -l 2
~~~


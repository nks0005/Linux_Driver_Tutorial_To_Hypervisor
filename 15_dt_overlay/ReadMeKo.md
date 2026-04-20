# 15_dt_overlay

이 예제는 **Raspberry Pi OS가 설치된 Raspberry Pi에서만** 동작합니다!

이번 예제에서는 첫 번째 디바이스 트리 오버레이(device tree overlay)를 추가하는 방법을 보여줍니다. 디바이스 트리는 사람이 읽기 쉬운 형태와, 컴파일된(머신이 읽는) 형태가 있습니다. 아래 표는 디바이스 트리의 다양한 형태를 요약합니다.

| File ending | Abbriviation               | Readable for |
| ----------- | -------------------------- | ------------ |
| dts         | Device Tree Source         | Human        | 
| dtsi        | Device Tree Source Include | Human        | 
| dtb         | Device Tree Binary         | Machine      |
| dtbo        | Device Tree Binary Overlay | Machine      |

## dts 설명(Explaining the dts)

디바이스 트리 오버레이를 위해 `my_overlay.dts`라는 디바이스 트리 소스 파일을 만듭니다.

파일 시작 부분에서 `/dts-v1/;`로 디바이스 트리 버전 정보를 제공하고, `/plugin/;`은 이 디바이스 트리 소스가 일반 디바이스 트리가 아니라 오버레이라는 것을 나타냅니다.

이제 디바이스 트리에 fragment를 추가할 수 있습니다. 각 fragment는 오버레이를 어디에 삽입할지에 대한 target 또는 target-path를 가집니다. 예를 들어 I2C 버스 1에 디바이스를 추가하려면 오버레이는 아래와 같이 생깁니다.

~~~
/{
	fragment@0 {
		target = <&i2c1>;
		__overlay__ {
            // Add device here 
        };
    };
};
~~~

하지만 이 예제에서는 디바이스 트리의 루트에 디바이스를 추가합니다. 따라서 target-path를 `/`로 설정합니다.

디바이스에는 디바이스 노드 이름이 필요하며, 이 예제에서는 `my_device`입니다. 노드 앞에는 라벨을 둘 수 있는데(여기서는 `my_dev_label`), 노드 이름 뒤에 `@`와 함께 주소를 붙일 수도 있습니다(예: `my_device@A0001000`). 이 주소의 의미는 디바이스가 어디에 추가되었는지에 따라 달라집니다.

- 프로세서의 물리 주소 공간에 매핑되는 경우: 물리 주소여야 함
- I2C 버스에 추가되는 경우: I2C 주소여야 함

주소는 선택 사항입니다.

그 다음 `my_device`에 여러 속성(property)을 추가합니다. 첫 번째는 `compatible` 문자열입니다. 이 문자열을 통해 디바이스 트리 노드와 사용할 드라이버가 연결됩니다. 보통 vendor 이름(brightlight)과 디바이스 이름(mydev)을 쉼표로 구분해 작성합니다.

다음 속성은 디바이스를 활성화합니다. `status`를 `"disable"`로 두면 디바이스는 비활성화되며, 이후 다른 오버레이로 활성화할 수 있습니다.

마지막 두 속성은 커스텀 속성입니다. 하나는 문자열, 다른 하나는 정수 변수입니다. 정수는 `<`와 `>`로 감싸야 합니다.

## 디바이스 트리 오버레이 컴파일(Compiling the Device Tree Overlay)

디바이스 트리 오버레이를 컴파일하려면 다음 명령을 실행합니다.

~~~
dtc -I dts -O dtb my_overlay.dts -o my_overlay.dtbo
~~~

`-I`는 입력 포맷(여기서는 디바이스 트리 소스), `-O`는 출력 포맷(디바이스 트리 바이너리)을 의미합니다. 그 뒤에 입력 파일명을 주고, `-o` 옵션 뒤에 출력 파일명을 지정합니다.

## 디바이스 트리 오버레이 적용(Applying the Device Tree Overlay)

Raspberry Pi OS에는 디바이스 트리 오버레이를 적용하기 위한 매우 간단한 도구가 포함되어 있습니다. 이 도구는 `dtoverlay`이며, 오버레이를 적용하려면 다음처럼 실행합니다.

~~~
sudo dtoverlay my_overlay.dtbo
~~~

디바이스 트리를 지원하는 다른 시스템에서는 오버레이를 로드하는 더 일반적인 방법들이 있지만, 여기서는 다루지 않습니다.

오버레이를 제거하려면 다음 명령을 사용할 수 있습니다.

~~~
sudo dtoverlay -R my_overlay
~~~

# 디바이스 트리 확인(Inspect the device tree)

오버레이를 추가한 뒤에는 `/proc/device-tree` 아래에 `my_device`라는 새 폴더가 생겨야 합니다. 이 폴더 안에는 디바이스의 각 property가 파일로 존재합니다.

~~~
$ ls /proc/device-tree/my_device/
compatible  int_var  name  status  string_var
~~~

`cat`이나 `hexdump`로 파일들을 확인하면, 디바이스 트리 소스에서 설정한 property 값들이 반영된 것을 볼 수 있습니다.


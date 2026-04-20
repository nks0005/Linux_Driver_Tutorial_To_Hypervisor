# 17_dt_read_property

이번 강의에서는 디바이스 트리 디바이스의 property(속성)를 파싱하는 방법을 배웁니다.

## 디바이스 트리 속성(Device Tree Properties)

이 예제의 디바이스 트리 디바이스에는 다음 속성들이 있습니다.

- compatible: 올바른 드라이버와 매칭하는 데 필요
- status: 디바이스 활성화/비활성화
- string_var: 커스텀 문자열 속성
- int_var: 커스텀 정수 속성

마지막 두 속성은 [Device Tree Specification](https://www.devicetree.org/specifications)에서 말하는 “비표준(nonstandard) 속성”입니다. 만약 해당 스펙에 더 compliant 하게 만들고 싶다면, 속성 이름 앞에 vendor prefix를 붙여야 합니다(예: `brightlight,string_var`). 일반적으로는 비표준 속성 사용을 가능한 피하는 것이 권장됩니다. DT 스펙에는 표준 속성 목록이 정리되어 있습니다.

## 드라이버 구현(Implementing the driver)

probe 함수에서 마지막 두 속성을 파싱합니다. 이를 위해 필요한 함수들은 `linux/property.h`에 정의되어 있습니다.

property를 읽기 전에 `device_property_present`로 property가 존재하는지 확인할 수 있습니다. 이 함수는 디바이스 포인터와 property 이름을 인자로 받으며, property가 없으면 0, 있으면 1을 반환합니다.

property 값을 읽을 때는 `device_property_read_<datatype>` 함수를 사용합니다. `<datatype>` 부분은 원하는 데이터 타입으로 바꿉니다.

- `int_var`를 읽기 위해 `device_property_32` 사용
- `string_var`를 읽기 위해 `device_property_string` 사용

인자는 `device_property_present`와 동일하고, 여기에 값을 저장할 포인터가 추가됩니다. 성공 시 0, 실패 시 음수 에러 코드를 반환합니다.

## 테스트(Testing)

~~~
# 코드 및 DT 오버레이 컴파일
make
# 드라이버 로드
sudo insmod my_dev_driver.ko

# DT 오버레이 삽입
sudo dtoverlay my_overlay.dtbo
# 이제 커널 로그를 확인하면 디바이스 속성이 출력되어야 합니다.
[#  574.316863] my_dev - int_var: 123
[#  574.316880] my_dev - string_var: Hello, DT!
~~~


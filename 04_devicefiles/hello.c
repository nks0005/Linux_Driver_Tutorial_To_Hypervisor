#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

/*
 * 왜 miscdevice를 쓰나?
 * - 문자 디바이스(/dev/xxx)를 가장 간단하게 “자동 생성”하고 싶을 때,
 *   커널의 misc 서브시스템을 이용하면 major/minor 관리와 /dev 노드 생성(udev 연동)이 매우 단순해집니다.
 * - 우리가 목표로 하는 건 insmod 후 /dev/hello 가 생기게 하는 것이고,
 *   misc_register()가 그 트리거(디바이스 등록 + uevent)를 만들어 줍니다.
 */

/*
 * hello_buf의 의미
 * - 이 드라이버가 제공하는 간단한 “디바이스 파일 데이터 영역”입니다.
 * - 유저가 /dev/hello 에 write()한 내용을 커널 버퍼에 저장하고,
 *   read()하면 그 버퍼를 다시 유저에게 전달합니다.
 * - 실전 드라이버라면 하드웨어 레지스터/큐/링버퍼 등을 다루겠지만,
 *   여기서는 device file 동작(읽기/쓰기 경로)을 보여주기 위한 최소 예제입니다.
 */
static char hello_buf[256];

/*
 * hello_read()
 * - 유저 프로세스가 read(fd, ...)를 호출하면 커널은 이 함수를 호출합니다.
 * - 중요한 점: user_buf는 “유저 공간 포인터”이므로 커널이 직접 memcpy 하면 안 되고,
 *   copy_to_user()로 안전하게 복사해야 합니다.
 * - *off(loff_t)는 파일 오프셋으로, 연속 read 동작에서 “어디까지 읽었는지”를 나타냅니다.
 */
static ssize_t hello_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	size_t avail, to_copy;

	if (*off >= sizeof(hello_buf))
		return 0;

	avail = sizeof(hello_buf) - *off;
	to_copy = min(len, avail);

	if (copy_to_user(user_buf, hello_buf + *off, to_copy))
		return -EFAULT;

	*off += to_copy;
	return to_copy;
}

/*
 * hello_write()
 * - 유저 프로세스가 write(fd, ...)를 호출하면 커널은 이 함수를 호출합니다.
 * - user_buf는 유저 공간이므로 copy_from_user()로 커널 버퍼로 가져옵니다.
 * - *off를 증가시키는 이유는 “파일처럼” 연속 write를 했을 때 다음 위치에 이어 쓰도록 하기 위해서입니다.
 * - 버퍼를 초과하면 -ENOSPC를 반환해서 “공간 부족”을 나타냅니다.
 */
static ssize_t hello_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	size_t avail, to_copy;

	if (*off >= sizeof(hello_buf))
		return -ENOSPC;

	avail = sizeof(hello_buf) - *off;
	to_copy = min(len, avail);

	if (copy_from_user(hello_buf + *off, user_buf, to_copy))
		return -EFAULT;

	*off += to_copy;
	return to_copy;
}

/*
 * file_operations (fops)
 * - (major,minor)로 라우팅된 뒤, 실제로 open/read/write/ioctl 등을 “어떤 함수로 처리할지”를 정의하는 테이블입니다.
 * - 유저가 /dev/hello 를 open()해서 얻는 fd는 내부적으로 이 fops에 연결됩니다.
 */
static const struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.read = hello_read,
	.write = hello_write,
};

/*
 * miscdevice
 * - misc 서브시스템에 등록할 디바이스 정의입니다.
 * - .minor = MISC_DYNAMIC_MINOR: minor를 커널이 비어있는 값으로 동적 할당
 * - .name = "hello": udev가 /dev/hello 라는 노드를 만들 때 쓰는 이름
 *   (즉, 윈도우의 “유저가 여는 이름”과 비슷한 역할을 여기서 합니다)
 */
static struct miscdevice hello_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hello",
	.fops = &hello_fops,
};

/*
 * my_init()
 * - insmod로 모듈이 로드될 때 호출됩니다.
 * - misc_register()를 호출하면:
 *   1) 커널에 문자 디바이스가 등록되고
 *   2) udev가 이를 감지하여 /dev/hello 노드를 생성할 수 있습니다(보통 자동으로 생성됨)
 */
static int __init my_init(void)
{
	int status;

	status = misc_register(&hello_miscdev);
	if (status) {
		pr_err("hello - misc_register failed: %d\n", status);
		return status;
	}

	pr_info("hello - created /dev/%s (minor=%d)\n", hello_miscdev.name, hello_miscdev.minor);
	return 0;
}

/*
 * my_exit()
 * - rmmod로 모듈이 언로드될 때 호출됩니다.
 * - misc_deregister()로 등록을 해제하면 /dev/hello 노드도 제거 대상이 됩니다(udev 동작).
 */
static void __exit my_exit(void)
{
	misc_deregister(&hello_miscdev);
	pr_info("hello - removed /dev/%s\n", hello_miscdev.name);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Linux Driver Tutorial");
MODULE_DESCRIPTION("miscdevice example creating /dev/hello");

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

static dev_t dev_nr;
static struct cdev my_cdev;

static ssize_t my_read(struct file *f, char __user *u, size_t l, loff_t *o)
{
	pr_info("hello_cdev - Read is called\n");
	return 0;
}


static struct file_operations fops = {
	.read = my_read
};

static int __init my_init(void)
{
	int status; /* 커널 API 호출들의 성공/실패(에러 코드) 저장 */
#ifdef STATIC_DEVNR
	dev_nr = STATIC_DEVNR; /* 사용할 (major, minor) 시작 번호를 고정으로 지정 */
	status = register_chrdev_region(dev_nr, MINORMASK + 1, "hello_cdev"); /* 해당 번호 범위를 선점(이미 사용 중이면 실패) */
#else
	status = alloc_chrdev_region(&dev_nr, 0, MINORMASK + 1, "hello_cdev"); /* 비어있는 번호 범위를 커널이 동적으로 할당 */
#endif
	if (status) { /* 번호 범위 확보(선점/할당) 실패 시 */
		pr_err("hello_cdev - Error reserving the region of device numbers\n"); /* 실패 로그 출력 */
		return status; /* 음수 에러 코드를 그대로 반환하여 init 실패 처리 */
	}

	cdev_init(&my_cdev, &fops); /* cdev 오브젝트를 초기화하고, 처리할 file_operations(fops) 연결 */
	my_cdev.owner = THIS_MODULE; /* 모듈 소유자 설정(모듈 언로드 안전성/참조 카운트 관련) */

	status = cdev_add(&my_cdev, dev_nr, MINORMASK + 1); /* dev_nr(major/minor 범위) ↔ cdev(fops) 바인딩 후 커널에 등록 */
	if (status) { /* cdev 등록 실패 시 */
		pr_err("hello_cdev - Error adding cdev\n"); /* 실패 로그 출력 */
		goto free_devnr; /* 이미 확보한 번호 범위를 반납하는 정리 루틴으로 점프 */
	}

	pr_info("hello_cdev - Registered a character device for Major %d starting with Minor %d\n",
		MAJOR(dev_nr), MINOR(dev_nr)); /* 실제로 잡힌 major/minor 시작 값을 로그로 확인 */
	return 0; /* init 성공 */

free_devnr:
	unregister_chrdev_region(dev_nr, MINORMASK + 1); /* 확보(선점/할당)했던 번호 범위를 반납 */
	return status; /* 실패 원인(status)을 그대로 반환 */
}

static void __exit my_exit(void)
{
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev_nr, MINORMASK + 1);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4Linux");
MODULE_DESCRIPTION("A sample driver for manually registering a character device");

# 디바이스 등록
> `register_chrdev()` : `전통적인` 문자 디바이스 등록 API, `major 번호`만 확보, `/dev` 노드 생성은 직접 처리 필요
> `misc_register()` : misc 서브시스템을 이용해 문자 디바이스를 등록해서 `minor를 자동 할당`해주고, `/dev/<name>` 노드가 udev로 자동 생성되기 쉬운 `간편 경로`

```c++
static const struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.read = hello_read,
	.write = hello_write,
};

static struct miscdevice hello_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hello",
	.fops = &hello_fops,
};

static int __init my_init(void)
{
	int status;

	status = misc_register(&hello_miscdev);

	return 0;
}

```
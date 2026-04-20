#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * hello_user.c
 *
 * - /dev/hello(디바이스 노드)를 open() 해서 fd(핸들)를 얻는다.
 * - write()로 커널 드라이버의 hello_write() 경로를 탄다.
 * - 많은 문자 디바이스는 lseek(파일 오프셋 이동)을 지원하지 않는다(ESPIPE/Illegal seek).
 *   따라서 “처음부터 다시 읽기”가 필요하면 close() 후 다시 open()해서 오프셋을 0으로 초기화한다.
 * - 다시 open()한 fd로 read()를 호출하면 커널 드라이버의 hello_read() 경로를 탄다.
 *
 * 참고: /dev/hello 권한이 root-only(crw-------)인 경우가 많아서
 *       실행은 sudo로 해야 할 수 있다.
 */

static void die(const char *what)
{
	fprintf(stderr, "%s: %s\n", what, strerror(errno));
	exit(1);
}

int main(void)
{
	const char *dev_path = "/dev/hello";
	const char *msg = "hello_user -> kernel driver\n";
	char buf[256];
	ssize_t n;

	int fd = open(dev_path, O_RDWR);
	if (fd < 0)
		die("open(/dev/hello)");

	n = write(fd, msg, strlen(msg));
	if (n < 0)
		die("write(/dev/hello)");

	close(fd);
	fd = open(dev_path, O_RDWR);
	if (fd < 0)
		die("re-open(/dev/hello)");

	memset(buf, 0, sizeof(buf));
	n = read(fd, buf, sizeof(buf) - 1);
	if (n < 0)
		die("read(/dev/hello)");

	printf("read %zd bytes: %s", n, buf);

	close(fd);
	return 0;
}


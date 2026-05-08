obj-m += hyper_box.o
hyper_box-objs := main.o cpuid.o msr.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

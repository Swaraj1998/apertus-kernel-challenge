obj-m+=cdev_checksum.o

CFLAGS_cdev_checksum.o := -Wall -Wextra

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

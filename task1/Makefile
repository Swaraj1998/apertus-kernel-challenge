obj-m+=csumdev.o

CFLAGS_csumdev.o := -Wall -Wextra

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

KBUILD_EXTRA_SYMBOLS := /home/jacobspigle/Documents/COP4600/COP4600ProgrammingAssignments/Assignment4/chardev-write/Module.symvers
obj-m += chardev-read.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
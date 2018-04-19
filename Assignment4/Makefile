all:
	cd chardev-write && make;
	cd chardev-write && sudo insmod chardev-write.ko && sudo mknod /dev/chardev-write c 247 0 && sudo chmod 666 /dev/chardev-write;
	cd chardev-read && make;
	cd chardev-read && sudo insmod chardev-read.ko && sudo mknod /dev/chardev-read c 246 0 && sudo chmod 666 /dev/chardev-read;

clean:
	sudo rmmod chardev-read;
	sudo rm /dev/chardev-read;
	cd chardev-read && make clean;
	sudo rmmod chardev-write;
	sudo rm /dev/chardev-write;
	cd chardev-write && make clean;
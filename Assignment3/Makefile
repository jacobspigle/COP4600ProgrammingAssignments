all:
	cd chardev-read && make;
	sudo insmod chardev-read.ko;
	sudo mknod /dev/chardev-read c 247;
	sudo chmod 666 /dev/chardev-read;
	cd ..;
	cd chardev-write && make;
	sudo insmod chardev-write.ko;
	sudo mknod /dev/chardev-write c 247;
	sudo chmod 666 /dev/chardev-write;

clean:
	sudo rmmod chardev-read;
	sudo rmmod chardev-write;
	sudo rm /dev/chardev-read;
	sudo rm /dev/chardev-write;
	cd chardev-read && make clean;
	cd chardev-write && make clean;
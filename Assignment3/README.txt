Compile:
    make all

//Testing
/* ***On the Virtual Box machine***
Open up Terminal to the Programming_Assignments_2 directory.
In order to build & run characterDeviceDriver.c, run the following:*/

make
sudo insmod characterDeviceDriver.ko

/*This will effectively build the necessary files, while
the insmod command runs the init_module function (registers device).
Next you have to actually create the device file.*/

//dmesg displays the kernel log, where all printk() statments output
//take note of the given majorNumber for the device
dmesg

//create the device file using the majorNumber
sudo mknod /dev/chardev c <majorNumber> 0

/*You have to give permissions to this device file to run
tests with it, use these commands:*/

sudo chmod 666 /dev/chardev
//note: chardev is the name of the device

/*From here you can start testing by feeding strings to
the driver.*/

//initiates 'write'
sudo echo "potion" >> /dev/chardev

//initiates 'device_read'
cat /dev/chardev

/*The device_write function prints the user input
to a variable, then outputs the number of characters.
The device_read function prints how many characters
were received.*/

//run to see results
dmesg

/*When you are finished testing, you must remove the module.*/
sudo rmmod characterDeviceDriver
sudo rm /dev/chardev
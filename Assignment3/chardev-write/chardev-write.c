#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

#include "../chardev.h"

extern int init_module(void);
extern void cleanup_module(void);
static int device_open(struct inode *, struct file* );
static int device_release(struct inode *, struct file* );
static ssize_t device_write(struct file *, const char* , size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev-write"

static int majorNumber;
static int deviceOpen = 0;

static struct file_operations fops = {
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

extern int init_module(void) {
    printk(KERN_INFO "Installing module.\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

    if (majorNumber < 0) {
        printk(KERN_ALERT "Device registration failed: %d\n", majorNumber);
        return majorNumber;
    }
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", majorNumber);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, majorNumber);
    printk(KERN_INFO "Try to cat and echo to the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    // init mutex
    mutex_init(&queue_mutex);

    return SUCCESS;
}

extern void cleanup_module(void) {
    printk(KERN_INFO "Removing module.\n");
     unregister_chrdev(majorNumber, DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *file){
    if (deviceOpen){
        return -EBUSY;
    }

    deviceOpen++;
    try_module_get(THIS_MODULE);

    printk(KERN_INFO "Device Opened.\n");
    return 0;
}
static int device_release(struct inode *inode, struct file *file){
    deviceOpen--;
    module_put(THIS_MODULE);

    printk(KERN_INFO "Device Released.\n");

    return 0;
}

static ssize_t device_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
    int buffer_space;
    int i;

    mutex_lock(&queue_mutex);

    buffer_space = BUFFER_SIZE - queueLen;

    if (buffer_space <= 0)
    	return length;

    if(length > buffer_space) {
        length = buffer_space;
    }

    for(i=0; i<length; i++) {
        queue[(head + queueLen + i) % BUFFER_SIZE] = buffer[i];
    }

    queueLen += length;

    mutex_unlock(&queue_mutex);

    printk(KERN_INFO "Device Write: received %zu characters\n", length);
    return length;
}
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file* );
static int device_release(struct inode *, struct file* );
static ssize_t device_read(struct file *, char* , size_t, loff_t *);
static ssize_t device_write(struct file *, const char* , size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUFFER_SIZE 1024

static int majorNumber;
static int deviceOpen = 0;

static char queue[BUFFER_SIZE];
static char output_buffer[BUFFER_SIZE];
static int head = 0;
static int queueLen = 0;

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

int init_module(void) {
    printk(KERN_INFO "Installing module.\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

    if (majorNumber < 0) {
        printk(KERN_ALERT "Device registration failed: %d\n", majorNumber);
        return majorNumber;
    }
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", majorNumber);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, majorNumber);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}
void cleanup_module(void) {
    printk(KERN_INFO "Removing module.\n");
     unregister_chrdev(majorNumber, DEVICE_NAME);
}
static int device_open(struct inode *inode, struct file *file){
    if (deviceOpen){
        return -EBUSY;
    }

    deviceOpen++;
    try_module_get(THIS_MODULE);

    printk(KERN_INFO "Hello potion seller.\n");
    return 0;
}
static int device_release(struct inode *inode, struct file *file){
    deviceOpen--;
    module_put(THIS_MODULE);

    return 0;
}

static ssize_t device_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
    int buffer_space = BUFFER_SIZE - queueLen;
    int i;
    if(length > buffer_space) {
        length = buffer_space;
    }

    printk(KERN_INFO "Length of message: %d", length);

    for(i=0; i<length; i++) {
        output_buffer[i] = queue[(head + i) % BUFFER_SIZE];
    }

    copy_to_user(buffer, output_buffer, length);

    head = (head + length) % BUFFER_SIZE;
    queueLen -= length;

    printk(KERN_INFO "chardev: sent %d characters\n", length);

    return length;
}
static ssize_t device_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
    int buffer_space = BUFFER_SIZE - queueLen;
    int i;
    if(length > buffer_space) {
        length = buffer_space;
    }

    for(i=0; i<length; i++) {
        queue[(head + queueLen + i) % BUFFER_SIZE] = buffer[i];
    }

    queueLen += length;

    printk(KERN_INFO "chardev: received %zu characters\n", length);
    return length;
}
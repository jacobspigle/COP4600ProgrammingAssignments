#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file* );
static int device_release(struct inode *, struct file* );
static ssize_t device_read(struct file *, char* , size_t, loff_t *);
static ssize_t device_write(struct file *, const char* , size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev-write"
#define BUFFER_SIZE 1024

static int majorNumber;
static int deviceOpen = 0;

extern char queue[BUFFER_SIZE];
extern int head = 0;
extern int queueLen = 0;

extern struct mutex queue_mutex;

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
    printk(KERN_INFO "Try to cat and echo to the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    // init mutex
    mutex_init(&my_mutex);

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

    printk(KERN_INFO "Device Opened.\n");
    return 0;
}
static int device_release(struct inode *inode, struct file *file){
    deviceOpen--;
    module_put(THIS_MODULE);

    printk(KERN_INFO "Device Released.\n");

    return 0;
}

static ssize_t device_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
    int num_read = 0;

    mutex_lock(&queue_mutex);

    if(length > queueLen) {
        length = queueLen;
    }

	while(queueLen && num_read < length) {
	    put_user(queue[head], buffer++);
	    queueLen--;
	    num_read++;
	    head = (head + 1) % BUFFER_SIZE;
	}

    mutex_unlock(&queue_mutex);

    printk(KERN_INFO "Device Read: sent %d characters\n", num_read);
    return num_read;
}
static ssize_t device_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
    mutex_lock(&queue_mutex);
    
    int buffer_space = BUFFER_SIZE - queueLen;
    int i;

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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

// #include "../chardev.h"

extern int init_module(void);
extern void cleanup_module(void);
static int device_open(struct inode *, struct file* );
static int device_release(struct inode *, struct file* );
static ssize_t device_write(struct file *, const char* , size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev-write"
#define BUFFER_SIZE 1024

static int majorNumber;
static int deviceOpen = 0;

// fucky stuff
char queue[BUFFER_SIZE];
int head = 0;
int queueLen = 0;
struct mutex queue_mutex;

const char *ucf_string = "Undefeated 2018 National Champions UCF";
const int ucf_length = 38;
int num_chars_checked = 0;


EXPORT_SYMBOL(queue);
EXPORT_SYMBOL(head);
EXPORT_SYMBOL(queueLen);
EXPORT_SYMBOL(queue_mutex);
EXPORT_SYMBOL(num_chars_checked);
// end of fucky stuff


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

static void replaceUCF(void)
{
    int i, j, k;
    int index_U, index_C, index_F;
    int index, ucf_chars_written = 0;
    int first_loop;
    
    printk(KERN_INFO "replaceUCF Qlen is: %d\n", queueLen);
    printk(KERN_INFO "Qlen-num_chars_checked is: %d\n", (queueLen - num_chars_checked));

    for(i=0; i<(queueLen - num_chars_checked); i++) {
        index_U = (head + i) % BUFFER_SIZE;
        index_C = (index_U + 1) % BUFFER_SIZE;
        index_F = (index_C + 1) % BUFFER_SIZE;

        index = index_U;

        if(index_C == head || index_F == head) {
            return;
        }

        if(queue[index_U] == 'U' && queue[index_C] == 'C' && queue[index_F] == 'F') {
            printk(KERN_INFO "\n\nTEST LOOK AT ME!!!!!\n\n");

            index = index_U;
            j = 0;
            first_loop = 1;

            if(BUFFER_SIZE - queueLen >= ucf_length - 3) {
                // head - ucf_length
                for(k=(head + BUFFER_SIZE - ucf_length) % BUFFER_SIZE; k>index_F; k--) {
                    queue[(k+1) % BUFFER_SIZE] = queue[k];
                }
            }

            while((first_loop || index != head) && j < ucf_length)
            {
                printk(KERN_INFO "while loop (j = %d) (index = %d)\n", j, index);
                first_loop = 0;
                queue[index] = ucf_string[j];
                queueLen++;
                index = (index + 1) % BUFFER_SIZE;
                ucf_chars_written++;
                num_chars_checked++;
                j++;
            }
        }
        else {
            printk(KERN_INFO "\n\nchars: %c %c %c\n\n", queue[index_U], queue[index_C], queue[index_F]);
        }
    }

    num_chars_checked += ucf_chars_written;
}

static ssize_t device_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
    int buffer_space;
    int i;

    mutex_lock(&queue_mutex);

    buffer_space = BUFFER_SIZE - queueLen;

    if (buffer_space <= 0) {
        mutex_unlock(&queue_mutex);
    	return length;
    }

    if(length > buffer_space) {
        length = buffer_space;
    }

    for(i=0; i<length; i++) {
        queue[(head + queueLen + i) % BUFFER_SIZE] = buffer[i];
    }

    queueLen += length;
    printk(KERN_INFO "device_write Qlen is: %d\n", queueLen);

    replaceUCF();

    mutex_unlock(&queue_mutex);

    printk(KERN_INFO "Device Write: received %zu characters\n", length);
    return length;
}
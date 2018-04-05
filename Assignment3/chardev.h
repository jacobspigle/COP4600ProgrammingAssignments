#include <linux/mutex.h>

extern struct mutex queue_mutex;

extern char queue[];
extern int head;
extern int queueLen;
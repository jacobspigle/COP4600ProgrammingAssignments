#include <linux/mutex.h>
#define BUFFER_SIZE 1024

extern struct mutex queue_mutex;

extern char queue[];
extern int head;
extern int queueLen;

#ifndef WRITE_CALLED
#define WRITE_CALLED

    char queue[BUFFER_SIZE];
    int head = 0;
    int queueLen = 0;

    struct mutex queue_mutex;
    
#endif
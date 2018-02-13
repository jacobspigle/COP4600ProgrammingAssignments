#include "processors.h"

typedef struct processQueue {
    process *p[MAX_PROCESSES];
    int head, count;
} processQueue;

void enqueue(processQueue *q, process *p)
{
    q->p[(q->head + q->count) % MAX_PROCESSES] = p;
    q->count++;
}

process *dequeue(processQueue *q)
{
    if(q->count == 0) {
        return NULL;
    }

    process *p = q->p[q->head];
    q->head = (q->head + 1) % MAX_PROCESSES;

    return p;
}

void runRoundRobin(FILE *ofp, process *processes, int numProcesses, int runfor, int quantum)
{
    sortByArrivalTime(processes, numProcesses);

    processQueue q;
    q.head = 0;
    q.count = 0;

    int processIndex = 0;
    int waitTimes[MAX_PROCESSES];
    int turnaroundTimes[MAX_PROCESSES];

    for(int i=0; i<MAX_PROCESSES; i++) {
        waitTimes[i] = 0;
        turnaroundTimes[i] = 0;
    }

    process *currentProcess = NULL;

    for(int timer=0; timer<runfor; timer++) {
        while(processIndex < numProcesses && processes[processIndex].sleep == timer) {
            processes[processIndex].lastTimeCheck = timer;
            enqueue(&q, &processes[processIndex]);
            processIndex++;
        }

        if(timer % quantum == 0) {
            turnaroundTimes[currentProcess->id] += (timer - currentProcess->lastTimeCheck);
            currentProcess->lastTimeCheck = timer;
            enqueue(&q, currentProcess);

            // get next process and see if this is its first time running
            currentProcess = dequeue(&q);
        }

        if(currentProcess == NULL) {
            fprintf(ofp, "Time %d: Idle\n", timer);
        }
        else {
            fprintf(ofp, "Time %d: %s arrived\n", timer, currentProcess->name);
        }
    }
}

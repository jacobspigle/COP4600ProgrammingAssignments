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
    char *names[MAX_PROCESSES];
    int waitTimes[MAX_PROCESSES];
    int turnaroundTimes[MAX_PROCESSES];

    for(int i=0; i<MAX_PROCESSES; i++) {
        waitTimes[i] = 0;
        turnaroundTimes[i] = 0;
    }

    process *currentProcess = NULL;

    for(int timer=0; timer<runfor; timer++) {
        while(processIndex < numProcesses && processes[processIndex].sleep == timer) {
            int id = processes[processIndex].id;
            names[id] = processes[processIndex].name;

            processes[processIndex].lastTimeCheck = timer;
            printStatusLine(ofp, timer, &processes[processIndex], "arrived");
            enqueue(&q, &processes[processIndex]);
            processIndex++;
        }

        bool processComplete = currentProcess->burst == 0;
        if(processComplete) {
            printStatusLine(ofp, timer, currentProcess, "finished");
        }

        if(processComplete || timer % quantum == 0) {
            // accumulate time spent running in turnaround slot
            turnaroundTimes[currentProcess->id] += (timer - currentProcess->lastTimeCheck);
            currentProcess->lastTimeCheck = timer;

            // if the process has more burst, queue it back up
            if(!processComplete) {
                enqueue(&q, currentProcess);
            }

            // get next process and see if this is its first time running
            currentProcess = dequeue(&q);
            printStatusLine(ofp, timer, currentProcess, "selected");

            // accumulate time spent waiting in wait slot and turnaround slot
            int timeSinceLastSelected = (timer - currentProcess->lastTimeCheck);
            waitTimes[currentProcess->id] += timeSinceLastSelected;
            turnaroundTimes[currentProcess->id] += timeSinceLastSelected;

            currentProcess->lastTimeCheck = timer;
        }

        if(currentProcess == NULL) {
            printStatusLine(ofp, timer, currentProcess, "idle");
        }
        else {
            printStatusLine(ofp, timer, currentProcess, "selected");
        }
    }

    fprintf(ofp, "Finished at time %d\n\n", runfor);

    for(int i=0; i<numProcesses; i++) {
        fprintf(ofp, "%s wait %d turnaround %d\n",
                        names[i], waitTimes[i], turnaroundTimes[i]);
    }
}

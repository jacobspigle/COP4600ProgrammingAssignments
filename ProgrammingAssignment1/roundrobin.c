#include "processors.h"

void runRoundRobin(FILE *ofp, process *processes, int numProcesses, int runfor, int quantum)
{
    if(DEBUG) printf("%s(n=%d, r=%d, q=%d)\n", __FUNCTION__, numProcesses, runfor, quantum);

    processQueue q;
    q.head = 0;
    q.count = 0;

    int processIndex = 0;

    for(int i = 0; i < numProcesses; i++) {
        processes[i].wait = 0;
        processes[i].turnaround = 0;
    }

    process *currentProcess = NULL;

    for(int timer=0; timer<runfor; timer++) {
        while(processIndex < numProcesses && processes[processIndex].arrival == timer) {
            int id = processes[processIndex].id;

            processes[processIndex].lastTimeCheck = timer;
            printStatusLine(ofp, timer, &processes[processIndex], "arrived");
            enqueue(&q, &processes[processIndex], numProcesses);
            processIndex++;
        }

        bool processComplete = currentProcess->burst == 0;
        if(processComplete) {
            printStatusLine(ofp, timer, currentProcess, "finished");
        }

        if(processComplete || timer % quantum == 0) {
            // accumulate time spent running in turnaround slot
            processes[currentProcess->id].turnaround += (timer - currentProcess->lastTimeCheck);
            currentProcess->lastTimeCheck = timer;

            // if the process has more burst, queue it back up
            if(!processComplete) {
                enqueue(&q, currentProcess, numProcesses);
            }

            // get next process and see if this is its first time running
            currentProcess = dequeue(&q, numProcesses);
            printStatusLine(ofp, timer, currentProcess, "selected");

            // accumulate time spent waiting in wait slot and turnaround slot
            int timeSinceLastSelected = (timer - currentProcess->lastTimeCheck);
            processes[currentProcess->id].wait += timeSinceLastSelected;
            processes[currentProcess->id].turnaround += timeSinceLastSelected;

            currentProcess->lastTimeCheck = timer;
        }

        if(currentProcess == NULL) {
            printStatusLine(ofp, timer, currentProcess, "idle");
        }
        else {
            printStatusLine(ofp, timer, currentProcess, "selected");
        }
    }

    printFooter(ofp, runfor, processes, numProcesses);
}

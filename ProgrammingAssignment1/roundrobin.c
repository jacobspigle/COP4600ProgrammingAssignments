#include "processors.h"

void runRoundRobin(FILE *ofp, process *processes, int numProcesses, int runfor, int quantum)
{
    if(DEBUG) printf("%s(n=%d, r=%d, q=%d)\n", __FUNCTION__, numProcesses, runfor, quantum);

    fprintf(ofp, "Quantum %d\n", quantum);
    
    processQueue q;
    q.head = 0;
    q.count = 0;

    int processIndex = 0;
    int quantumTimer = quantum;

    process *currentProcess = NULL;

    for(int timer=0; timer<runfor; timer++) {
        // Queue up new arrivals
        while(processIndex < numProcesses && processes[processIndex].arrival == timer) {
            processes[processIndex].timeStamp = timer;
            processes[processIndex].initialBurst = processes[processIndex].burst;

            printStatusLine(ofp, timer, &processes[processIndex], "arrived");
            enqueue(&q, &processes[processIndex], numProcesses);
            processIndex++;
        }
        
        // Handle current process state
        bool getNext = false;
        if(currentProcess == NULL) {
            getNext = true;
        }
        else {
            if(currentProcess->burst == 0) {
                currentProcess->turnaround = timer - currentProcess->arrival;
                currentProcess->wait = currentProcess->turnaround - currentProcess->initialBurst;
                printStatusLine(ofp, timer, currentProcess, "finished");
                getNext = true;
            }
            else if(quantumTimer == 0) {
                enqueue(&q, currentProcess);
                getNext = true;
            }
        }

        // Get next if necessary
        if(getNext) {
            currentProcess = dequeue(&q);

            if(currentProcess != NULL) {
                printStatusLine(ofp, timer, currentProcess, "selected");
                quantumTimer = quantum;
            }
        }

        // Do work
        if(currentProcess == NULL) {
            printStatusLine(ofp, timer, NULL, "idle");
        }
        else {
            currentProcess->burst--;
            quantumTimer--;
        }

        if(currentProcess == NULL) {
            printStatusLine(ofp, timer, currentProcess, "idle");
        }
    }

    printFooter(ofp, runfor, processes, numProcesses);
}

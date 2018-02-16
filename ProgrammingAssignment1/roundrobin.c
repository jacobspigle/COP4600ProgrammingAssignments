#include "processors.h"

void runRoundRobin(FILE *ofp, process *processes, int numProcesses, int runfor, int quantum)
{
    if(DEBUG) printf("%s(n=%d, r=%d, q=%d)\n", __FUNCTION__, numProcesses, runfor, quantum);

    processQueue q;
    q.head = 0;
    q.count = 0;

    int processIndex = 0;
    int quantumTimer = quantum;

    process *currentProcess = NULL;

    for(int timer=0; timer<runfor; timer++) {
        printf("%d\n", timer);
        // Queue up new arrivals
        while(processIndex < numProcesses && processes[processIndex].arrival == timer) {
            processes[processIndex].timeStamp = timer;
            processes[processIndex].initialBurst = processes[processIndex].burst;

            printStatusLine(ofp, timer, &processes[processIndex], "arrived");
            enqueue(&q, &processes[processIndex], numProcesses);
            processIndex++;
        }

        quantumTimer--;
        bool processComplete = false;

        // run process for 1 time unit
        if(currentProcess != NULL) {
            if(currentProcess->burst <= 0) {
                fprintf(stderr, "Process remaining time invalid.\n");
                return;
            }

            currentProcess->burst--;

            processComplete = (currentProcess->burst == 0);
            if(processComplete) {
                // calculate turnaround and wait for completed process
                currentProcess->turnaround = timer - currentProcess->arrival;
                currentProcess->wait = currentProcess->turnaround - currentProcess->initialBurst;

                printStatusLine(ofp, timer, currentProcess, "finished");
            }
        }

        if(processComplete || quantumTimer == 0) {
            // If process is not complete, requeue
            if(currentProcess != NULL && !processComplete) {
                enqueue(&q, currentProcess, numProcesses);
            }

            // get next process
            currentProcess = dequeue(&q, numProcesses);
            quantumTimer = quantum;

            if(currentProcess != NULL) {
                printStatusLine(ofp, timer, currentProcess, "selected");
            }
        }

        if(currentProcess == NULL) {
            printStatusLine(ofp, timer, currentProcess, "idle");
        }
    }

    printFooter(ofp, runfor, processes, numProcesses);
}

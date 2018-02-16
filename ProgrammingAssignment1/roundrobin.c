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
            processes[processIndex].initialBurst = processes[processIndex].burst;

            printStatusLine(ofp, timer, &processes[processIndex], "arrived");
            enqueue(&q, &processes[processIndex]);
            processIndex++;
        }

        if(currentProcess == NULL) {
            // get next process
            currentProcess = dequeue(&q);

            if(currentProcess != NULL) {
                quantumTimer = quantum;
                printStatusLine(ofp, timer, currentProcess, "selected");
            }
            else {
                // TODO: make this work
                printStatusLine(ofp, timer, currentProcess, "idle");
            }
        }
        else {
            // run process, then check if its done or if the quantum is over
            if(currentProcess->burst <= 0) {
                fprintf(stderr, "Process remaining time invalid %d => %d.\n", currentProcess->initialBurst, currentProcess->burst);
                return;
            }

            currentProcess->burst--;
            quantumTimer--;

            if(currentProcess->burst == 0) {
                // calculate turnaround and wait for completed process
                currentProcess->turnaround = timer - currentProcess->arrival + 1;
                currentProcess->wait = currentProcess->turnaround - currentProcess->initialBurst;

                printStatusLine(ofp, timer, currentProcess, "finished");

                currentProcess = NULL;
            }
            else if(quantumTimer == 0) {
                // requeue
                enqueue(&q, currentProcess);
            }
        }
    }

    printFooter(ofp, runfor, processes, numProcesses);
}

#include "processors.h"


void runFCFS(FILE *ofp, process *processes, int numProcesses, int runfor)
{
    printf("first come\n");
    int i;

    for(i = 0; i < numProcesses; i++){
        printf("arrival %d \n", processes[i].sleep);
    }

    int tick = 0;
    int count = 0;
    int numCompleted = 0;
    bool running = false;
    process* queue[numProcesses];

    while(numCompleted < numProcesses){
        
        // Check Arrivals
        for(i = 0; i < numProcesses; i++){
            if(tick == processes[i].sleep ){
                queue[count++] = &processes[i];
                printStatusLine( ofp, tick, queue[count-1], "arrived");
                printf("Arrival %d \n", processes[i].sleep);
            }
        }
        // Run Burst
        if(running){
             queue[numCompleted]->burst--;
             if( queue[numCompleted]->burst <= 0){
                printStatusLine( ofp, tick, queue[numCompleted], "finished");
                running = false;
                numCompleted++;
                printf("Completed on tick %d\n", tick);
             }
        }
        // Select
        if (running == false && numCompleted < numProcesses && tick >= queue[numCompleted]->sleep){
            running = true;
            printStatusLine( ofp, tick, queue[numCompleted], "selected");
            printf("Starting %s in tick %d\n", queue[numCompleted]->name, tick);
        }

        // Avoid End Tick
        if(numCompleted < numProcesses){
            tick++;
            printf("tick %d\n", tick);
        }
        if(tick > 50)
            break;
    }

    printFooter(ofp, tick, processes, numProcesses);
}
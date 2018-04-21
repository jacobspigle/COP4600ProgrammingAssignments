#include "processors.h"


void runFCFS(FILE *ofp, process *processes, int numProcesses, int runfor)
{
    int i;

    int tick = 0;
    int count = 0;
    int index = -1;
    bool running = false;
    process* queue[numProcesses];

    while(tick < runfor+1){
        
        // Check Arrivals
        for(i = 0; i < numProcesses; i++){
            if(tick == processes[i].arrival ){
                if(index == -1)
                    index++;
                queue[count++] = &processes[i];
                printStatusLine( ofp, tick, queue[count-1], "arrived");
            }
        }
        // Run Burst
        if(running){
            queue[index]->burst--;
            queue[index]->turnaround++;
            if( queue[index]->burst <= 0){
                printStatusLine( ofp, tick, queue[index], "finished");
                running = false;
                index++;
            //    printf("Completed on tick %d\n", tick);
            }
        }
        // Select
        if(running == false){
            if ( index >= 0 && index < numProcesses  && tick >= queue[index]->arrival){
                running = true;
                printStatusLine( ofp, tick, queue[index], "selected");
              //  printf("Starting %s in tick %d\n", queue[index]->name, tick);
            }
            else if( tick < runfor ){
                printStatusLine( ofp, tick, queue[index], "idle");
            }
        }

        if(index >= 0 )
            for(i = index + 1; i < count; i++){
                queue[i]->wait++;
                queue[i]->turnaround++;
            }

        tick++;
       // printf("tick %d\n", tick);
    }

    printFooter(ofp, tick-1, processes, numProcesses);
}
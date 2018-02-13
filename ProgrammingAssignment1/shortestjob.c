#include "processors.h"

void runShortestJobFirst(FILE *ofp, process *processes, int numProcesses, int runfor)
{
    //initialize tick
    int tick = 0;
    process *shortestBurst = &processes[0];

    //go through 'runfor' times
    while (tick < runfor)
    {
        //find process with smallest burst
        for (int i = 0; i < numProcesses; i++)
        {
            //notify if process has arrived
            if (processes[i].sleep == tick)
            {
                printStatusLine(ofp, tick, processes[i], "arrived");
            }

            //only if processes[i] still has > 0 burst AND is smaller than shortestBurst process
            if ((processes[i].burst < shortestBurst->burst) && (processes[i].burst != 0))
            {
                shortestBurst = &processes[i];
            }
        }

        //select process with shortest burst
        printStatusLine(ofp, tick, *shortestBurst, "selected");
        
        //decrement burst count
        shortestBurst->burst--;

        //process is finished if burst is now 0
        if (shortestBurst->burst == 0)
        {
            printStatusLine(ofp, ++tick, *shortestBurst, "finished");
        }
        
        //increment tick
        tick++;
    }
}
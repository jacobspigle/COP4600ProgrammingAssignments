#include "processors.h"

void runShortestJobFirst(FILE *ofp, process *processes, int numProcesses, int runfor)
{
    //initialize tick
    int tick = 0;

    //set to null, assign when a process has arrived
    process *shortestBurst = NULL;
    
    //go through 'runfor' times
    while (tick < runfor)
    {
        //find process with smallest burst
        for (int i = 0; i < numProcesses; i++)
        {
            //check if processes have arrived, set as shortestBurst to contradict later
            if (processes[i].arrival == tick)
            {
                printStatusLine(ofp, tick, &processes[i], "arrived");
                shortestBurst = &processes[i];
            }

            if (shortestBurst != NULL)
            {
                if (((processes[i].burst < shortestBurst->burst)                //processes[i] has shorter burst AND
                    && (processes[i].burst != 0)                                //processes[i] has not finished AND
                    && (processes[i].arrival <= tick))                          //processes[i] has arrived
                    || (shortestBurst->burst == 0 && processes[i].burst != 0))  //OR shortestBurst has finished AND processes[i] has not
                {
                    //set new shortestBurst
                    shortestBurst = &processes[i];
                }
            }
        }

        //check to see if any processes have arrived
        if (shortestBurst != NULL)
        {
            //select process with shortest burst
            printStatusLine(ofp, tick, shortestBurst, "selected");
            
            //decrement burst count
            shortestBurst->burst--;

            //process is finished if burst is now 0
            if (shortestBurst->burst == 0)
            {
                printStatusLine(ofp, tick + 1, shortestBurst, "finished");
            }
        }

        //increment tick
        tick++;
    }

    //function completed
    printFooter(ofp, runfor, processes, numProcesses);
}
#include "processors.h"

void runShortestJobFirst(FILE *ofp, process *processes, int numProcesses, int runfor)
{
    //initialize tick
    int tick = 0;
    processQueue queue;
    queue.head = 0;
    queue.count = 0;
    int processIndex;
    //go through 'runfor' times
    while (tick < runfor)
    {
        /*
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
        */
        //if a process arrives this tick
        while(processIndex < numProcesses && processes[processIndex].arrival == tick)
        {
            printStatusLine(ofp, tick, &processes[processIndex], "arrived");
            enqueue(&queue, &processes[processIndex], numProcesses);
            processIndex++;
        }
        
        //if queue is empty
        if (queue.count == 0)
        {           
            printStatusLine(ofp, tick, 0, "idle");
            fprintf(stderr, "what");
        }
        else
        {
            //sort new process in queue
            int i = 0;
            while (queue.p[queue.head + i]->burst > queue.p[queue.head + i + 1]->burst)
            {
                process *temp = queue.p[queue.head + i]; 
                queue.p[queue.head + i] = queue.p[queue.head + i + 1];
                queue.p[queue.head + i + 1] = temp;
            }

            //select process with shortest burst
            printStatusLine(ofp, tick, queue.p[queue.head], "selected");

            //decrement burst count
            queue.p[queue.head]->burst--;

            //process is finished if burst is now 0
            if (queue.p[queue.head]->burst == 0)
            {
                printStatusLine(ofp, tick + 1, queue.p[queue.head], "finished");
                dequeue(&queue, numProcesses);
            }
        }

        //increment tick
        tick++;
    }

    //function completed
    printFooter(ofp, runfor, processes, numProcesses);
}
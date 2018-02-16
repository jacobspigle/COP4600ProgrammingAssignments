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
        //if a process arrives this tick
        while(processIndex < numProcesses && processes[processIndex].arrival == tick)
        {
            printStatusLine(ofp, tick, &processes[processIndex], "arrived");
            enqueue(&queue, &processes[processIndex]);
            queue.p[queue.head]->initialBurst = queue.p[queue.head]->burst;
            processIndex++;
        }
        
        //if queue is empty
        if (queue.count == 0)
        {
            printStatusLine(ofp, tick, 0, "idle");
        }
        else
        {
            for (int i = 0; i < queue.count - 1; i++)
            {
                //sort new process in queue
                if (queue.p[queue.head + i]->burst >= queue.p[queue.head + i + 1]->burst)
                {
                    process *temp = queue.p[queue.head + i]; 
                    queue.p[queue.head + i] = queue.p[queue.head + i + 1];
                    queue.p[queue.head + i + 1] = temp;
                }
            }

            //select process with shortest burst
            printStatusLine(ofp, tick, queue.p[queue.head], "selected");

            //decrement burst count, increment turnaround
            queue.p[queue.head]->burst--;

            //process is finished if burst is now 0
            if (queue.p[queue.head]->burst == 0)
            {
                printStatusLine(ofp, tick + 1, queue.p[queue.head], "finished");
                queue.p[queue.head]->turnaround = tick - queue.p[queue.head]->arrival + 1;
                queue.p[queue.head]->wait = queue.p[queue.head]->turnaround - queue.p[queue.head]->initialBurst;
                dequeue(&queue);
            }
        }

        //increment tick
        tick++;
    }

    //function completed
    printFooter(ofp, runfor, processes, numProcesses);
}
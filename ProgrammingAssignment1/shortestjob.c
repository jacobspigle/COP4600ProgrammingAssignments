#include "processors.h"

void runShortestJobFirst(FILE *ofp, process *processes, int numProcesses, int runfor)
{
    //initialize tick
    int tick = 0;
    processQueue queue;
    queue.head = 0;
    queue.count = 0;
    int processIndex;
    bool newHead = false;

    //go through 'runfor' times
    while (tick < runfor)
    {
        //if a process arrives this tick
        while(processIndex < numProcesses && processes[processIndex].arrival == tick)
        {
            //first process arrives
            if (processIndex == 0)
            {
                newHead = true;
            }

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
            process *checkHead = queue.p[queue.head];

            for (int j = 0; j < queue.count - 1; j++)
            {
                //sort new process in queue
                if (queue.p[queue.head + j]->burst >= queue.p[queue.head + j + 1]->burst)
                {
                    process *temp = queue.p[queue.head + j]; 
                    queue.p[queue.head + j] = queue.p[queue.head + j + 1];
                    queue.p[queue.head + j + 1] = temp;
                }
            }
            for (int i = 0; i < queue.count - 1; i++)
            {
                //if queue.head has changed
                if (checkHead != queue.p[queue.head])
                {
                    newHead = true;
                }

                //increment other processes wait & turnaround times
                queue.p[queue.head + i + 1]->wait++;
                queue.p[queue.head + i + 1]->turnaround++;
            }

            //only print "selected" when selection has changed
            if (newHead == true)
            {
                //select process with shortest burst
                printStatusLine(ofp, tick, queue.p[queue.head], "selected");
                newHead = false;
            }

            //decrement burst count, increment turnaround
            queue.p[queue.head]->burst--;
            queue.p[queue.head]->turnaround++;

            //process is finished if burst is now 0
            if (queue.p[queue.head]->burst == 0)
            {
                printStatusLine(ofp, tick + 1, queue.p[queue.head], "finished");
                dequeue(&queue);
                newHead = true;
            }
        }

        //increment tick
        tick++;
    }

    //function completed
    printFooter(ofp, runfor, processes, numProcesses);
}
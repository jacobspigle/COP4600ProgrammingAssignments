#include "processors.h"

typedef struct processQueue {
    process p[MAX_PROCESSES];
    struct processNode *next;
} processNode;

void runRoundRobin(FILE *ofp, process *processes, int numProcesses, int runfor, int quantum)
{
    sortByArrivalTime(processes, numProcesses);

    int processIndex = 0;

    for(int i=0; i<runfor; i++) {

    }
}
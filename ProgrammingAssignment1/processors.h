#ifndef _PROCESSORS_H_
#define _PROCESSORS_H_

enum error {
    ERR_NONE=0, ERR_FOPEN_FAILED, ERR_MAX_PROCESSES_EXCEEDED
};

enum algorithm {
    ALG_FCFS, ALG_P_SHORTEST, ALG_RROBIN
};

typedef struct process {
    int id;
    char name[100];
    int sleep;
    int burst;
} process;

void runFCFS(FILE *ofp, process *processes, int numProcesses);
void runShortestJobFirst(FILE *ofp, process *processes, int numProcesses);
void runRoundRobin(FILE *ofp, process *processes, int numProcesses);

#endif
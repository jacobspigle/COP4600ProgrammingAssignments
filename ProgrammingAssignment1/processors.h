#ifndef _PROCESSORS_H_
#define _PROCESSORS_H_

#include <stdio.h>

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

void runFCFS(FILE *ofp, process *processes, int numProcesses, int runfor);
void runShortestJobFirst(FILE *ofp, process *processes, int numProcesses, int runfor);
void runRoundRobin(FILE *ofp, process *processes, int numProcesses, int runfor, int quantum);
void printStatusLine(FILE *ofp, int time, process p, int burst, char *state);

#endif
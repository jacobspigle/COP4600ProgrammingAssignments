#ifndef _PROCESSORS_H_
#define _PROCESSORS_H_

#include <stdio.h>
#include <stdbool.h>

#define MAX_PROCESSES 1024
#define DEBUG true

enum error {
    ERR_NONE=0, ERR_FOPEN_FAILED, ERR_MAX_PROCESSES_EXCEEDED
};

enum algorithm {
    ALG_FCFS, ALG_P_SHORTEST, ALG_RROBIN
};

typedef struct process {
    int id;
    char name[100];
    int arrival;
    int initialBurst;
    int burst;

    int remainingTime;
    int wait;
    int turnaround;
    int timeStamp;
} process;

typedef struct processQueue {
    process *p[MAX_PROCESSES];
    int head, count;
} processQueue;

void runFCFS(FILE *ofp, process *processes, int numProcesses, int runfor);
void runShortestJobFirst(FILE *ofp, process *processes, int numProcesses, int runfor);
void runRoundRobin(FILE *ofp, process *processes, int numProcesses, int runfor, int quantum);
void printStatusLine(FILE *ofp, int time, process *p, char *state);
void printFooter(FILE *ofp, int time, process *processes, int numProcesses);
void sortByArrivalTime(process *processes);
void enqueue(processQueue *q, process *p);
process *dequeue(processQueue *q);

#endif
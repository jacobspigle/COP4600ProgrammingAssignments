//Jacob Spigle
//Hunter Figueroa
//David Ferguson
//COP4600
//Programming Assignment 1

#include <stdio.h>
#include <string.h>

#include "processors.h"

int stringToAlg(char *str);
void printAlgLine(FILE *fp, int alg);
void runProcessor(FILE *ofp, process *processes, int numProcesses, int runfor, int alg, int quantum);
void eatLine(FILE *fp);

char buffer[256];

int main(int argc, char **argv)
{
    const char *inputFileName = "processes.in";
    const char *outputFileName = "processes.out";
    
    FILE *fp = fopen(inputFileName, "r");
    FILE *ofp = fopen(outputFileName, "w");

    if(fp == NULL || ofp == NULL) {
        fprintf(stderr, "Failed to open file(s)\n");
        return ERR_FOPEN_FAILED;
    }

    // Parse Input File
    int numProcesses;
    process processes[MAX_PROCESSES];
    int runfor;
    int alg;
    int quantum;

    fscanf(fp, "processcount %d", &numProcesses);
    fgets(buffer, 256, fp);

    if(numProcesses > MAX_PROCESSES) {
        fprintf(stderr, "Maximum number of processes exceeded.\n");
        return ERR_MAX_PROCESSES_EXCEEDED;
    }
    if(DEBUG) printf("processes %d \n", numProcesses );

    fscanf(fp, "runfor %d", &runfor);
    fgets(buffer, 256, fp);
    if(DEBUG) printf("RunFor %d \n", runfor );


    char algString[100];
    fscanf(fp, "use %s", algString);
    fgets(buffer, 256, fp);
    alg = stringToAlg(algString);
    if(DEBUG) printf("ALG %d end \n", alg );

    if(alg == ALG_RROBIN) {
        fscanf(fp, "quantum %d", &quantum);
        if(DEBUG) printf("Quantum %d \n", quantum );
    }
    fgets(buffer, 256, fp);

    for(int i=0; i<numProcesses; i++) {
        processes[i].id = i;
        fscanf(fp, "process name %s arrival %d burst %d",
                        processes[i].name, &processes[i].arrival, &processes[i].burst);
        fgets(buffer, 256, fp);
    }
    
    // Write Header to Output File
    if(DEBUG) fprintf(ofp, "%d processes\n", numProcesses);
    printAlgLine(ofp, alg);

    if(alg == ALG_RROBIN) {
        fprintf(ofp, "Quantum %d\n", quantum);
    }

    fprintf(ofp, "\n\n");

    if(DEBUG) {
        // TODO: print processes
    }

    sortByArrivalTime(processes, numProcesses);
    runProcessor(ofp, processes, numProcesses, runfor, alg, quantum);

    fclose(fp);
    fclose(ofp); 
}

void runProcessor(FILE *ofp, process *processes, int numProcesses, int runfor, int alg, int quantum)
{
    printf("Run Processor\n");
    switch(alg)
    {
        case ALG_FCFS:
            runFCFS(ofp, processes, numProcesses, runfor);
            break;
            
        case ALG_P_SHORTEST:
            runShortestJobFirst(ofp, processes, numProcesses, runfor);
            break;
            
        case ALG_RROBIN:
            for(int i=0; i<numProcesses; i++) {
                printf("Case RR  Name: %s, Sleep: %d, Burst: %d, NP:%d RF:%d Q:%d\n", processes[i].name, processes[i].wait, processes[i].burst, numProcesses, runfor, quantum);
            }

            runRoundRobin(ofp, processes, numProcesses, runfor, quantum);
            break;
            
        default:
            fprintf(stderr, "Invalid algorithm enum: %d\n", alg);
    }
}

int stringToAlg(char *str)
{
    printf("ALG %s", str);
    if(strcmp(str, "fcfs") == 0) {
        return ALG_FCFS;
    }
    else if(strcmp(str, "sjf") == 0) {
        return ALG_P_SHORTEST;
    }
    else if(strcmp(str, "rr") == 0) {
        return ALG_RROBIN;
    }
    else {
        return -1;
    }
}

void printAlgLine(FILE *fp, int alg)
{
    fprintf(fp, "Using ");

    switch(alg)
    {
        case ALG_FCFS:
            fprintf(fp, "First Come First Served");
            break;
        
        case ALG_P_SHORTEST:
            fprintf(fp, "Shortest Job First");
            break;

        case ALG_RROBIN:
            fprintf(fp, "Round-Robin");
            break;

        default:
            fprintf(fp, "ERROR");
    }

    fprintf(fp, "\n");
}

//call this function inside ****.c
//takes current tick (time), the processName, the burst of that process, and what state the process is in.
void printStatusLine(FILE *ofp, int time, process *p, char *state)
{    
    fprintf(ofp, "Time %d: ", time);

    if (strcmp(state, "idle"))
    {
        fprintf(ofp, "%s", p->name);

        if (strcmp(state, "arrived") == 0)
        {
            fprintf(ofp, " arrived\n");
        }
        else if (strcmp(state, "selected") == 0)
        {
            fprintf(ofp, " selected (burst %d)\n", p->burst);
        }
        else if (strcmp(state, "finished") == 0)
        {
            fprintf(ofp, " finished\n");
        }
    }
    else
        fprintf(ofp, "Idle\n");
}

//call this function inside ****.c
//output 'finished' time and turnaround time for each process
void printFooter(FILE *ofp, int runfor, process processes[], int numProcesses)
{
    fprintf(ofp, "Finished at time %d\n\n", runfor);

    for (int i = 0; i < numProcesses; i++)
    {
        fprintf(ofp, "%s wait %d turnaround %d\n", processes[i].name, processes[i].wait, processes[i].turnaround);
    }
}

void sortByArrivalTime(process *processes, int numProcesses)
{
    // bubble sort
    for(int i=0; i<numProcesses; i++) {

        bool swapped = false;
        for(int j=0; j<numProcesses-i-1; j++) {
            int prevarrival = processes[j].arrival;
            int nextarrival = processes[j+1].arrival;

            // sort identical arrival times by ascending id
            if(prevarrival > nextarrival || (prevarrival == nextarrival && processes[j].id > processes[j+1].id)) {
                // swap
                process temp = processes[j];
                processes[j] = processes[j+1];
                processes[j+1] = temp;

                swapped = true;
            }
        }

        if(swapped == false) {
            break;
        }
    }
}

void eatLine(FILE *fp)
{
    char ch = fgetc(fp);
    while(ch != '\n' && ch != EOF)
    {
        ch = fgetc(fp);
    }
}

void enqueue(processQueue *q, process *p)
{
    q->p[(q->head + q->count) % MAX_PROCESSES] = p;
    q->count++;
}

process *dequeue(processQueue *q)
{
    if(q->count == 0) {
        return NULL;
    }

    process *p = q->p[q->head];
    q->head = (q->head + 1) % MAX_PROCESSES;
    q->count--;

    return p;
}

void printQueue(FILE *ofp, processQueue *q)
{ 
    fprintf(ofp, "{");
    for(int i=0; i<q->count; i++) {
        process *p = q->p[(q->head + i) % MAX_PROCESSES];
        fprintf(ofp, "%s(burst %d) ", p->name, p->burst);
    }
    fprintf(ofp, "}\n");
}
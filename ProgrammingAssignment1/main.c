//Jacob Spigle
//Hunter Figueroa
//David Ferguson
//COP4600
//Programming Assignment 1

#include <stdio.h>

#include "processors.h"

#define MAX_PROCESSES 1024

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

    fscanf(fp, "processcount %d[^\n]*\n", &numProcesses);

    if(numProcesses > MAX_PROCESSES) {
        fprintf(stderr, "Maximum number of processes exceeded.\n");
        return ERR_MAX_PROCESSES_EXCEEDED;
    }

    fscanf(fp, "\\S*runfor %d[^\n]*\n", &runfor);

    char algString[100];
    fscanf(fp, "\\S*use %s[^\n]*\n", algString);
    alg = stringToAlg(algString);

    if(alg == ALG_RROBIN) {
        fscanf(fp, "\\S*quantum %d[^\n]*\n", quantum);
    }

    for(int i=0; i<numProcesses; i++) {
        processes[i].id = i;
        fscanf(fp, "\\S*process name %s arrival %d burst %d[^\n]*\n",
                        processes[i].name, processes[i].sleep, processes[i].burst);
    }
    
    // Write Header to Output File
    fprintf(ofp, "%d processes\n", numProcesses);
    printAlgLine(ofp, alg);

    if(alg == ALG_RROBIN) {
        fprintf(ofp, "Quantum %d\n", quantum);
    }

    fprintf(ofp, "\n");

    runProcessor(ofp, processes, numProcesses, alg);

    fclose(fp);
    fclose(ofp);
}

void runProcessor(FILE *ofp, process *processes, int numProcesses, int alg)
{
    switch(alg)
    {
        case ALG_FCFS:
            runFCFS(ofp, processes, numProcesses);
            break;
            
        case ALG_P_SHORTEST:
            runShortestJobFirst(ofp, processes, numProcesses);
            break;
            
        case ALG_RROBIN:
            runRoundRobin(ofp, processes, numProcesses);
            break;
            
        default:
            fprintf(stderr, "Invalid algorithm code.\n");
    }
}

int stringToAlg(char *str)
{
    if(strncmp(str, "fcfs") == 0) {
        return ALG_FCFS;
    }
    else if(strncmp(str, "sjf")) {
        return ALG_P_SHORTEST;
    }
    else if(strncmp(str, "rr") == 0) {
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
            fprintf(fp, "First-Come-First-Served");
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
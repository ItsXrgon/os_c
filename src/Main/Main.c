#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/queue.h"
#include "../Headers/structs.h"
#include "../Headers/memory.h"
#include "../Headers/Instructions.h"
#include "../Headers/scheduler.h"
#include "../Headers/mutex.h"


#define NUM_PROCESSES 3

int current_time = 0;
int time_quantum;

PCB processTable[MAX_PROCESSES];
Queue readyQueue;
Mutex userInput;
Mutex userOutput;
Mutex file;

// Function to intialize process table 
void initialize_process_table()
    {
    for (int i = 0; i < NUM_PROCESSES; i++)
        {
        processTable[i].pid = i + 1;
        processTable[i].state = NEW;
        processTable[i].counter = 0;
        processTable[i].lower_bound = -1;
        processTable[i].upper_bound = -1;
        processTable[i].arrival_time = 0;
        char filename[100];
        sprintf(filename, "/home/youssef/GUC/OS/M2/os_c/src/Program_%d.txt", i + 1);
        strcpy(processTable[i].filename, filename);
        }
    }
void print_PCB() {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("PID: %d\n", processTable[i].pid);
        printf("State: %d\n", processTable[i].state);
        printf("Counter: %d\n", processTable[i].counter);
        printf("Lower Bound: %d\n", processTable[i].lower_bound);
        printf("Upper Bound: %d\n", processTable[i].upper_bound);
        printf("Arrival Time: %d\n", processTable[i].arrival_time);
        printf("Filename: %s\n", processTable[i].filename);
        printf("\n");
        }

    }

// Main function
int main()
    {
    // Initialize memory
    initialize_memory();

    // Initialize mutex
    initMutex(&userInput);
    initMutex(&userOutput);
    initMutex(&file);

    // Initialize process table
    initialize_process_table();

    // Initialize ready queue
    initQueue(&readyQueue);

    // Get time quantum

    printf("Enter the time quantum: ");
    scanf("%d", &time_quantum);

    // Get arrival time for each process
    for (int i = 0; i < NUM_PROCESSES; i++)
        {
        printf("Enter arrival time for process %d: ", i + 1);
        scanf("%d", &processTable[i].arrival_time);
        }

    schedule(processTable, NUM_PROCESSES);

    print_memory();

    return 0;
    }

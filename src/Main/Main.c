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

int ready_queue[10];
int front = -1, rear = -1;
// Main function
int main()
    {
    // Initialize memory
    initialize_memory();
    initialize_mutex();



    PCB processes[NUM_PROCESSES] = {
        {1, "READY", 0, 0, 0, 0, "/home/youssef/GUC/OS/M2/os_c/src/Program_1.txt"},
        {2, "READY", 0, 0, 0, 0, "/home/youssef/GUC/OS/M2/os_c/src/Program_2.txt"},
        {3, "READY", 0, 0, 0, 0, "/home/youssef/GUC/OS/M2/os_c/src/Program_3.txt"} };

    printf("Enter the time quantum: ");
    scanf("%d", &time_quantum);

    for (int i = 0; i < NUM_PROCESSES; i++)
        {
        printf("Enter arrival time for process %d: ", i + 1);
        scanf("%d", &((processes + i)->arrival_time));
        }

    run_scheduler(processes, NUM_PROCESSES);

    print_memory();

    return 0;
    }

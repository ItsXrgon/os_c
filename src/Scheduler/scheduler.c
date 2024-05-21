#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/queue.h"
#include "../Headers/structs.h"
#include "../Headers/memory.h"
#include "../Headers/Instructions.h"

extern Memory memory;
extern int current_time;
extern Queue readyQueue;
extern int time_quantum;

void update_PCB(PCB* pcb) {
    if (pcb == NULL) {
        printf("Error: Process not found\n");
        return;
        }
    if (pcb->state == READY || pcb->state == RUNNING) {
        enqueue(&readyQueue, pcb);
        }
    }
void Increment_counter(PCB* pcb) {
    pcb->counter++;
    }

// Function to execute a program in memory
void execute_program(PCB* pcb)
    {
    if (pcb == NULL)
        {
        printf("Error: Process not found\n");
        return;
        }

    if (pcb->state == TERMINATED || pcb->state == BLOCKED)
        {
        return;
        }
    // Check if the program has finished execution
    if (pcb->counter >= pcb->upper_bound)
        {
        pcb->state = TERMINATED;
        return;
        }

    int instruction_index = pcb->lower_bound + pcb->counter;
    if (memory.memory_blocks[instruction_index].name[0] != '\0') {
        char instruction[100];
        strcpy(instruction, memory.memory_blocks[instruction_index].data);
        printf("Executing instruction: %s\n", instruction);
        execute_instruction(instruction, pcb);
        pcb->counter++;
        }



    update_Memory_PCB(pcb);
    }

// Function to add arriving processes to the ready queue
void add_arriving_processes(PCB processes[], int num_processes)
    {
    for (int i = 0; i < num_processes; i++)
        {
        if (processes[i].state == NEW && processes[i].arrival_time == current_time)
            {
            LoadProgram(processes[i].filename, processes + i);
            enqueue(&readyQueue, processes + i);
            printf("Process %d has arrived\n", processes[i].pid);
            }
        }
    }

int All_Terminated(PCB processes[], int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].state != TERMINATED) {
            return 0;
            }
        }
    return 1;
    }

void schedule(PCB processes[], int num_processes) {
    PCB* currentProcess = NULL;
    int timeSlice = 0;
    printf("Starting scheduling\n");
    while (All_Terminated(processes, num_processes) == 0) {
        printf("Current time: %d\n\n", current_time);
        add_arriving_processes(processes, num_processes);
        if (currentProcess == NULL || currentProcess->state != RUNNING) {
            if (!isQueueEmpty(&readyQueue)) {
                currentProcess = dequeue(&readyQueue);
                currentProcess->state = RUNNING;
                timeSlice = 0;
                printf("Process %d is now RUNNING\n", currentProcess->pid);
                }
            }

        if (currentProcess != NULL) {
            execute_program(currentProcess);
            timeSlice++;

            if (timeSlice >= time_quantum) {
                currentProcess->state = READY;
                enqueue(&readyQueue, currentProcess);
                printf("Process %d is now READY\n", currentProcess->pid);
                currentProcess = NULL;
                }

            if (currentProcess != NULL && currentProcess->counter >= (currentProcess->upper_bound - currentProcess->lower_bound)) {
                currentProcess->state = TERMINATED;
                printf("Process %d is now TERMINATED\n", currentProcess->pid);
                currentProcess = NULL;
                }

            }

        current_time++;
        }
    }

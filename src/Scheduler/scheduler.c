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
extern PCB processTable[MAX_PROCESSES];


void update_PCB(PCB* pcb) {
    if (pcb == NULL) {
        printf("Error: Process not found\n");
        return;
        }
    if (pcb->state == READY || pcb->state == RUNNING) {
        enqueue(&readyQueue, pcb);
        }
    }

// Function to execute a program in memory
void execute_program(PCB* pcb)
    {
    if (pcb == NULL)
        {
        printf("Error: Process not found\n");
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
void add_arriving_processes()
    {
    for (int i = 0; i < MAX_PROCESSES; i++)
        {
        if (processTable[i].state == NEW && processTable[i].arrival_time == current_time)
            {
            LoadProgram(processTable[i].filename, processTable + i);
            enqueue(&readyQueue, processTable + i);
            processTable[i].state = READY;
            printf("Process %d has arrived\n", processTable[i].pid);
            }
        }
    }
void updatePCB(PCB* pcb) {
    pcb->counter++;
    }

int All_Terminated() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processTable[i].state != TERMINATED) {
            return 0;
            }
        }
    printf("All processes have terminated\n");
    return 1;
    }

void schedule() {
    PCB* currentProcess = NULL;
    int timeSlice = 0;
    printf("Starting scheduling\n");
    while (All_Terminated() == 0 || !isQueueEmpty(&readyQueue)) {
        printf("Current time: %d\n \n", current_time);
        add_arriving_processes();
        if (currentProcess == NULL || currentProcess->state != READY) {
            if (!isQueueEmpty(&readyQueue)) {
                currentProcess = dequeue(&readyQueue);
                currentProcess->state = RUNNING;
                timeSlice = 0;
                printf("Process %d is now RUNNING\n", currentProcess->pid);
                }
            }

        if (currentProcess != NULL) {
            currentProcess->state = RUNNING;
            execute_program(currentProcess);
            updatePCB(currentProcess);
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

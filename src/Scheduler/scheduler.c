#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/queue.h"
#include "../Headers/structs.h"
#include "../Headers/memory.h"
#include "../Headers/Instructions.h"

extern Memory memory;
extern int current_time;
extern int ready_queue[10];
extern int front, rear;

void update_PCB(PCB* pcb) {
    if (pcb == NULL) {
        printf("Error: Process not found\n");
        return;
        }
    if (strcmp(pcb->state, "DEAD") != 0 || strcmp(pcb->state, "BLOCKED") != 0) {
        enqueue(&(pcb->pid), ready_queue, &front, &rear);
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

    if (strcmp(pcb->state, "DEAD") == 0 || strcmp(pcb->state, "BLOCKED") == 0)
        {
        return;
        }
    // Check if the program has finished execution
    if (pcb->counter >= (pcb->upper_bound - pcb->lower_bound))
        {
        strcpy(pcb->state, "DEAD");
        }

    // Execute only one instruction per call
    if (pcb->counter < pcb->upper_bound - pcb->lower_bound)
        {
        int instruction_index = pcb->lower_bound + pcb->counter;
        if (memory.memory_blocks[instruction_index].name[0] != '\0')
            {
            char instruction[100];
            strcpy(instruction, memory.memory_blocks[instruction_index].data);
            printf("Executing instruction: %s\n", instruction);
            execute_instruction(instruction, pcb);
            pcb->counter++;
            }
        }



    update_Memory_PCB(pcb);
    }

// Function to add arriving processes to the ready queue
void add_arriving_processes(PCB processes[], int num_processes)
    {
    for (int i = 0; i < num_processes; i++)
        {
        if (strcmp(processes[i].state, "READY") == 0 && processes[i].arrival_time == current_time)
            {
            LoadProgram(processes[i].filename, processes + i);
            enqueue(&(processes[i].pid), ready_queue, &front, &rear);
            printf("Process %d has arrived\n", processes[i].pid);
            }
        }
    }

// Function to run the scheduler
void run_scheduler(PCB processes[], int num_processes)
    {
    int processes_remaining = num_processes;

    while (1)
        {
        printf("\nTime: %d\n", current_time);
        add_arriving_processes(processes, num_processes);
        int pid = -1;
        if (isEmpty(&front, &rear))
            {
            printf("No processes in the ready queue\n");
            current_time++;
            continue;
            }
        else
            {
            pid = dequeue(ready_queue, &front, &rear);
            }


        PCB* process = &processes[pid - 1];

        if (strcmp(process->state, "DEAD") == 0)
            {
            printf("Process %d has completed execution\n", process->pid);
            update_PCB(process);
            continue;
            }

        if (strcmp(process->state, "BLOCKED") == 0)
            {
            printf("Process %d is blocked\n", process->pid);
            update_PCB(process);
            continue;
            }


        if (strcmp(process->state, "READY") == 0 || strcmp(process->state, "RUNNING") == 0)
            {
            strcpy(process->state, "RUNNING");
            printf("\nExecuting process %d\n", process->pid);
            execute_program(process);
            }

        update_PCB(process);

        int all_dead = 1;
        for (int i = 0; i < num_processes; i++)
            {
            if (strcmp(processes[i].state, "DEAD") != 0)
                {
                all_dead = 0;
                break;
                }
            }

        current_time++;
        }
    }
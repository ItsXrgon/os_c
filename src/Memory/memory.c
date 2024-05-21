#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/queue.h"
#include "../Headers/structs.h"

#define str(x) #x
#define xstr(x) str(x)



// Global variables for memory and time
Memory memory;
int mem_start = 0;


void update_Memory_PCB(PCB* pcb)
    {
    for (int i = pcb->upper_bound + 3; i < pcb->upper_bound + 8; i++)
        {
        if (strcmp(memory.memory_blocks[i].name, "PID") == 0 && atoi(memory.memory_blocks[i].data) == pcb->pid)
            {
            sprintf(memory.memory_blocks[i + 1].data, "%s", str(pcb->state));
            sprintf(memory.memory_blocks[i + 2].data, "%d", pcb->counter);
            sprintf(memory.memory_blocks[i + 3].data, "%d", pcb->lower_bound);
            sprintf(memory.memory_blocks[i + 4].data, "%d", pcb->upper_bound);
            }
        }
    }


// Function to initialize memory
void initialize_memory()
    {
    for (int i = 0; i < MEMORY_SIZE; i++)
        {
        memory.memory_blocks[i].name[0] = '\0';
        memory.memory_blocks[i].data[0] = '\0';
        }
    }

// Function to print memory
void print_memory()
    {
    printf("Memory: \n");
    for (int i = 0; i < MEMORY_SIZE; i++)
        {
        if (memory.memory_blocks[i].name[0] != '\0')
            {
            printf("Name: %s  Value: %s\n", memory.memory_blocks[i].name, memory.memory_blocks[i].data);
            }
        }
    }

// Function to search for a variable in memory and return its data
char* search_variable(PCB* pcb, char* name)
    {
    for (int i = pcb->upper_bound; i < pcb->upper_bound + 3; i++)
        {
        if (strcmp(memory.memory_blocks[i].name, name) == 0)
            {
            return memory.memory_blocks[i].data;
            }
        }
    return NULL;
    }

// Function to write PCB to memory
void write_pcb_to_memory(PCB* pcb)
    {
    strcpy(memory.memory_blocks[mem_start].name, "PID");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb->pid);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "State");
    strcpy(memory.memory_blocks[mem_start].data, str(pcb->state));
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "Counter");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb->counter);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "Lower_Bound");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb->lower_bound);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "Upper_Bound");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb->upper_bound);
    mem_start++;
    }


// Function to load a program
void LoadProgram(char* filename, PCB* pcb)
    {
    FILE* file = fopen(filename, "r");
    if (file == NULL)
        {
        printf("Error: File not found\n");
        return;
        }
    pcb->state = READY;
    pcb->counter = 0;
    pcb->lower_bound = mem_start;
    char instruction[100];
    int i = 0;
    while (fgets(instruction, 100, file))
        {
        char instruction_name[100];
        sprintf(instruction_name, "Instruction_%d", i);
        strcpy(memory.memory_blocks[mem_start].name, instruction_name);
        strcpy(memory.memory_blocks[mem_start].data, instruction);
        mem_start++;
        i++;
        }
    fclose(file);
    pcb->upper_bound = mem_start;
    strcpy(memory.memory_blocks[mem_start].name, "a");
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "b");
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "c");
    mem_start++;

    // Assign PCB to memory
    write_pcb_to_memory(pcb);
    }
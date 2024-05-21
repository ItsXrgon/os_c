#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/queue.h"
#include "../Headers/structs.h"
#include "../Headers/memory.h"


extern Memory memory;
extern Queue readyQueue;

extern Mutex userInput;
extern Mutex userOutput;
extern Mutex file;


Mutex* chooseMutex(char* arg1)
    {
    if (strcmp(arg1, "userInput") == 0)
        {
        return &userInput;
        }
    else if (strcmp(arg1, "userOutput") == 0)
        {
        return &userOutput;
        }
    else if (strcmp(arg1, "file") == 0)
        {
        return &file;
        }
    else
        {
        printf("Invalid mutex\n");
        return NULL;
        }
    }

void initMutex(Mutex* m) {
    m->value = 1;
    initQueue(&m->blockedQueue);
    }

// Function to read data from a file
char* read_file(char* filename)
    {
    FILE* file = fopen(filename, "r");
    if (file == NULL)
        {
        printf("Error: File not found\n");
        return NULL;
        }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)malloc(length);
    if (buffer)
        {
        fread(buffer, 1, length, file);
        }
    fclose(file);
    return buffer;
    }



// Function to assign value to a variable
void assign(char* arg1, char* arg2, char* arg3, PCB* pcb)
    {
    int index = -1;
    if (strcmp(arg2, "input") == 0)
        {
        printf("Enter value for %s: ", arg1);
        scanf("%s", arg2);
        if (strcmp(arg1, "a") == 0)
            {
            index = pcb->upper_bound;
            }
        else if (strcmp(arg1, "b") == 0)
            {
            index = pcb->upper_bound + 1;
            }
        else if (strcmp(arg1, "c") == 0)
            {
            index = pcb->upper_bound + 2;
            }
        if (index != -1)
            {
            strcpy(memory.memory_blocks[index].data, arg2);
            }
        }
    else
        {
        char filename[100];
        if (strcmp(arg2, "readFile") == 0)
            {
            char* filename = search_variable(pcb, arg3);
            if (filename == NULL)
                {
                printf("Invalid filename %s \n", arg3);
                return;
                }
            char* output = read_file(filename);
            printf("Output: %s\n", output);
            if (output != NULL)
                {
                if (strcmp(arg1, "a") == 0)
                    {
                    index = pcb->upper_bound;
                    }
                else if (strcmp(arg1, "b") == 0)
                    {
                    index = pcb->upper_bound + 1;
                    }
                else if (strcmp(arg1, "c") == 0)
                    {
                    index = pcb->upper_bound + 2;
                    }
                if (index != -1)
                    {
                    strcpy(memory.memory_blocks[index].data, output);
                    }
                }
            }
        }
    }

// Function to print a string
void print(char* arg1, PCB* pcb)
    {
    char* data = search_variable(pcb, arg1);
    if (data != NULL)
        {
        printf("%s\n", data);
        }
    }

// Function to print a range of numbers
void print_from_to(char* arg1, char* arg2, PCB* pcb)
    {
    char* from = search_variable(pcb, arg1);
    char* to = search_variable(pcb, arg2);
    if (from == NULL || to == NULL)
        {
        printf("Invalid range\n");
        return;
        }

    for (int i = atoi(from); i <= atoi(to); i++)
        {
        printf("%d\n", i);
        }
    }

// Function to write data to a file
void write_file(char* arg1, char* arg2, PCB* pcb)
    {
    char* filename = search_variable(pcb, arg1);
    char* data = search_variable(pcb, arg2);
    if (data != NULL && filename != NULL)
        {
        FILE* file = fopen(filename, "w");
        if (file != NULL)
            {
            fputs(data, file);
            fclose(file);
            }
        }
    }


void semWait(Mutex* m, PCB* pcb) {
    if (m->value > 0) {
        m->value--;
        }
    else {
        pcb->state = BLOCKED;
        enqueue(&m->blockedQueue, pcb);
        }
    }

void semSignal(Mutex* m) {
    if (!isQueueEmpty(&m->blockedQueue)) {
        PCB* pcb = dequeue(&m->blockedQueue);
        pcb->state = READY;
        enqueue(&readyQueue, pcb);
        }
    else {
        m->value++;
        }
    }

// Function to execute an instruction
void execute_instruction(char* instruction, PCB* pcb)
    {
    char command[20], arg1[20], arg2[20], arg3[20];
    if (sscanf(instruction, "%s %s %s %s", command, arg1, arg2, arg3) < 1)
        {
        return;
        }

    if (strcmp(command, "assign") == 0)
        {
        assign(arg1, arg2, arg3, pcb);
        pcb->state = READY;
        }
    else if (strcmp(command, "print") == 0)
        {
        print(arg1, pcb);
        pcb->state = READY;
        }
    else if (strcmp(command, "printFromTo") == 0)
        {
        print_from_to(arg1, arg2, pcb);
        pcb->state = READY;
        }
    else if (strcmp(command, "writeFile") == 0)
        {
        write_file(arg1, arg2, pcb);
        pcb->state = READY;
        }
    else if (strcmp(command, "semWait") == 0)
        {
        Mutex* m = chooseMutex(arg1);
        semWait(m, pcb);
        }
    else if (strcmp(command, "semSignal") == 0)
        {
        Mutex* m = chooseMutex(arg1);
        semSignal(m);
        }
    else
        {
        printf("Invalid instruction\n");
        }
    }
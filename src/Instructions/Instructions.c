#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/queue.h"
#include "../Headers/structs.h"
#include "../Headers/memory.h"


extern Memory memory;

mutex userInputMutex;
mutex userOutputMutex;
mutex fileMutex;


mutex* chooseMutex(char* arg1)
    {
    if (strcmp(arg1, "userInput") == 0)
        {
        return &userInputMutex;
        }
    else if (strcmp(arg1, "userOutput") == 0)
        {
        return &userOutputMutex;
        }
    else if (strcmp(arg1, "file") == 0)
        {
        return &fileMutex;
        }
    else
        {
        printf("Invalid mutex\n");
        return NULL;
        }
    }

void initialize_mutex()
    {
    userInputMutex.locked = 0;
    userOutputMutex.locked = 0;
    fileMutex.locked = 0;

    userInputMutex.front = -1;
    userInputMutex.rear = -1;

    userOutputMutex.front = -1;
    userOutputMutex.rear = -1;

    fileMutex.front = -1;
    fileMutex.rear = -1;


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

// Function to lock a mutex
void semWait(mutex* m, PCB* pcb)
    {
    if (m->locked)
        {
        if (m->owner_id == pcb->pid)
            {
            printf("Process %d already owns the mutex\n", pcb->pid);
            return;
            }
        if (ifExists(m->blocked_queue, &(m->front), &(m->rear), pcb->pid))
            {
            printf("Process %d is already blocked\n", pcb->pid);
            return;
            }
        else {
            enqueue(&(pcb->pid), m->blocked_queue, &(m->front), &(m->rear));
            strcpy(pcb->state, "BLOCKED");
            printf("Process %d is blocked\n", pcb->pid);
            }
        }

    else
        {
        strcpy(pcb->state, "RUNNING");
        m->locked = 1;
        m->owner_id = pcb->pid;
        printf("Mutex ownership acquired by process %d\n", pcb->pid);
        }

    }

void semSignal(mutex* m, PCB* pcb)
    {
    if (m->locked)
        {
        if (m->owner_id != pcb->pid)
            {
            printf("Process %d does not own the mutex\n", pcb->pid);
            return;
            }
        m->locked = 0;
        m->owner_id = -1;
        strcpy(pcb->state, "READY");
        printf("Mutex ownership released by process %d\n", pcb->pid);
        int next_pid = dequeue(m->blocked_queue, &(m->front), &(m->rear));
        if (next_pid != -1)
            {
            strcpy(pcb->state, "READY");
            m->owner_id = next_pid;
            m->locked = 1;
            printf("Mutex ownership transferred to process %d\n", next_pid);
            }
        }
    else
        {

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
        strcpy(pcb->state, "READY");
        }
    else if (strcmp(command, "print") == 0)
        {
        print(arg1, pcb);
        strcpy(pcb->state, "READY");
        }
    else if (strcmp(command, "printFromTo") == 0)
        {
        print_from_to(arg1, arg2, pcb);
        strcpy(pcb->state, "READY");
        }
    else if (strcmp(command, "writeFile") == 0)
        {
        write_file(arg1, arg2, pcb);
        strcpy(pcb->state, "READY");
        }
    else if (strcmp(command, "semWait") == 0)
        {
        mutex* m = chooseMutex(arg1);
        semWait(m, pcb);
        }
    else if (strcmp(command, "semSignal") == 0)
        {
        mutex* m = chooseMutex(arg1);
        semSignal(m, pcb);
        }
    else
        {
        printf("Invalid instruction\n");
        }
    }
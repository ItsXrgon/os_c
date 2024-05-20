#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 60
#define NUM_PROCESSES 3

typedef struct
{
    char name[20];  // Name of the variable or instruction
    char data[200]; // Data of the variable or instruction
} MemoryWord;

typedef struct
{
    MemoryWord memory_blocks[MEMORY_SIZE]; // Memory divided into memory words
} Memory;

typedef struct
{
    int pid;         // Process ID
    char state[20];  // Process State
    int priority;    // Current Priority
    int counter;     // Program Counter
    int lower_bound; // Lower Bound of the process’ space in the memory
    int upper_bound; // Upper Bound of the process’ space in the memory
    int arrival_time;
    int quantum; // Time quantum for the current level
    char filename[100];
} PCB;

typedef struct
{
    int semaphore;
} mutex;

mutex userInputMutex;
mutex userOutputMutex;
mutex fileMutex;

Memory memory;
int mem_start = 0;
int current_time = 0;

int queue_time_quantum[4] = {1, 2, 4, 8}; // Time quantum for each level

typedef struct
{
    PCB *processes[NUM_PROCESSES];
    int front;
    int rear;
} Queue;

Queue ready_queues[4]; // Four levels of queues

void initialize_memory()
{
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        memory.memory_blocks[i].name[0] = '\0';
        memory.memory_blocks[i].data[0] = '\0';
    }
}

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

void read_file(const char *filename, char *buffer, size_t size)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }
    size_t len = fread(buffer, 1, size, file);
    buffer[len] = '\0';
    fclose(file);
}

void assign(char *arg1, char *arg2)
{
    if (strcmp(arg2, "input") == 0)
    {
        printf("Enter value for %s: ", arg1);
        scanf("%s", arg2);
    }
    else
    {
        char command[20], var[20];
        sscanf(arg2, "%s %s", command, var);
        if (strcmp(command, "readFile") == 0)
        {
            char buffer[100];
            read_file(var, buffer, sizeof(buffer));
            strcpy(arg2, buffer);
        }
    }

    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        if (strcmp(memory.memory_blocks[i].name, arg1) == 0)
        {
            strcpy(memory.memory_blocks[i].data, arg2);
            return;
        }
    }

    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        if (memory.memory_blocks[i].name[0] == '\0')
        {
            strcpy(memory.memory_blocks[i].name, arg1);
            strcpy(memory.memory_blocks[i].data, arg2);
            return;
        }
    }
}

char *search_memory(char *name)
{
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        if (strcmp(memory.memory_blocks[i].name, name) == 0)
        {
            return memory.memory_blocks[i].data;
        }
    }
    return NULL;
}

void print(char *arg1)
{
    char *data = search_memory(arg1);
    if (data != NULL)
    {
        printf("%s \n", data);
    }
}

void print_from_to(int from, int to)
{
    for (int i = from; i <= to; i++)
    {
        printf("%d\n", i);
    }
}

void write_file(char *arg1, char *arg2)
{
    char *data = search_memory(arg1);
    if (data != NULL)
    {
        FILE *file = fopen(arg2, "w");
        if (file != NULL)
        {
            fputs(data, file);
            fclose(file);
        }
    }
}

void semWait(char *arg1)
{
    if (strcmp(arg1, "userInputMutex") == 0)
    {
        while (userInputMutex.semaphore <= 0)
            ;
        userInputMutex.semaphore--;
    }
    else if (strcmp(arg1, "userOutputMutex") == 0)
    {
        while (userOutputMutex.semaphore <= 0)
            ;
        userOutputMutex.semaphore--;
    }
    else if (strcmp(arg1, "fileMutex") == 0)
    {
        while (fileMutex.semaphore <= 0)
            ;
        fileMutex.semaphore--;
    }
}

void semSignal(char *arg1)
{
    if (strcmp(arg1, "userInputMutex") == 0)
    {
        userInputMutex.semaphore++;
    }
    else if (strcmp(arg1, "userOutputMutex") == 0)
    {
        userOutputMutex.semaphore++;
    }
    else if (strcmp(arg1, "fileMutex") == 0)
    {
        fileMutex.semaphore++;
    }
}

void execute_instruction(char *instruction)
{
    char command[20], arg1[20], arg2[20];
    if (sscanf(instruction, "%s %s %s", command, arg1, arg2) < 1)
    {
        return;
    }

    if (strcmp(command, "assign") == 0)
    {
        assign(arg1, arg2);
    }
    else if (strcmp(command, "print") == 0)
    {
        print(arg1);
    }
    else if (strcmp(command, "printFromTo") == 0)
    {
        print_from_to(atoi(arg1), atoi(arg2));
    }
    else if (strcmp(command, "writefile") == 0)
    {
        write_file(arg1, arg2);
    }
    else if (strcmp(command, "semWait") == 0)
    {
        semWait(arg1);
    }
    else if (strcmp(command, "semSignal") == 0)
    {
        semSignal(arg1);
    }
    else
    {
        printf("Invalid instruction\n");
    }
}

void write_pcb_to_memory(PCB *pcb)
{
    if (mem_start + 6 > MEMORY_SIZE)
    {
        printf("Error: Not enough memory to write PCB\n");
        return;
    }
    strcpy(memory.memory_blocks[mem_start].name, "PID");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb->pid);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "State");
    strcpy(memory.memory_blocks[mem_start].data, pcb->state);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "Priority");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb->priority);
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

void LoadProgram(char *filename, int pID)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: File not found\n");
        return;
    }
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    pcb->pid = pID;
    strcpy(pcb->state, "READY");
    pcb->counter = 0;
    pcb->priority = 0;
    pcb->lower_bound = mem_start;
    pcb->quantum = queue_time_quantum[0];
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
    mem_start = mem_start + (6 - (mem_start % 6)); // Align to the next block
    write_pcb_to_memory(pcb);
    ready_queues[0].processes[ready_queues[0].rear] = pcb;
    ready_queues[0].rear = (ready_queues[0].rear + 1) % NUM_PROCESSES;
}

void initialize_queues()
{
    for (int i = 0; i < 4; i++)
    {
        ready_queues[i].front = 0;
        ready_queues[i].rear = 0;
    }
}

void scheduler()
{
    while (1)
    {
        int executed = 0;
        for (int i = 0; i < 4; i++)
        {
            if (ready_queues[i].front != ready_queues[i].rear)
            {
                PCB *pcb = ready_queues[i].processes[ready_queues[i].front];
                ready_queues[i].front = (ready_queues[i].front + 1) % NUM_PROCESSES;
                pcb->state[0] = '\0';
                strcpy(pcb->state, "RUNNING");
                printf("Executing process %d at level %d with quantum %d\n", pcb->pid, i, pcb->quantum);
                char instruction_name[100];
                sprintf(instruction_name, "Instruction_%d", pcb->counter);
                char *instruction = search_memory(instruction_name);
                if (instruction != NULL)
                {
                    execute_instruction(instruction);
                    pcb->counter++;
                    pcb->quantum--;
                }
                if (pcb->quantum > 0 && pcb->counter < pcb->upper_bound - pcb->lower_bound)
                {
                    ready_queues[i].processes[ready_queues[i].rear] = pcb;
                    ready_queues[i].rear = (ready_queues[i].rear + 1) % NUM_PROCESSES;
                }
                else if (pcb->counter >= pcb->upper_bound - pcb->lower_bound)
                {
                    strcpy(pcb->state, "DEAD");
                    printf("Process %d finished execution\n", pcb->pid);
                }
                else
                {
                    pcb->quantum = queue_time_quantum[i + 1];
                    if (i < 3)
                    {
                        ready_queues[i + 1].processes[ready_queues[i + 1].rear] = pcb;
                        ready_queues[i + 1].rear = (ready_queues[i + 1].rear + 1) % NUM_PROCESSES;
                    }
                    else
                    {
                        ready_queues[i].processes[ready_queues[i].rear] = pcb;
                        ready_queues[i].rear = (ready_queues[i].rear + 1) % NUM_PROCESSES;
                    }
                }
                executed = 1;
                break;
            }
        }
        if (!executed)
        {
            break;
        }
    }
}

int main()
{
    initialize_memory();
    initialize_queues();

    userInputMutex.semaphore = 1;
    userOutputMutex.semaphore = 1;
    fileMutex.semaphore = 1;

    LoadProgram("Program_1.txt", 1);
    LoadProgram("Program_2.txt", 2);
    LoadProgram("Program_3.txt", 3);

    scheduler();

    print_memory();

    return 0;
}

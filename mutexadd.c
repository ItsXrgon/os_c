#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_PROCESSES 3
#define MEMORY_SIZE 60
#define QUEUE_SIZE 10
#define MAX_PRIORITY 4

int ready_queues[MAX_PRIORITY][QUEUE_SIZE];
int front[MAX_PRIORITY], rear[MAX_PRIORITY];
int blocked_queue[QUEUE_SIZE];
int blocked_front = -1, blocked_rear = -1;

// Memory
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
    int pid;            // Process ID
    char state[20];     // Process State
    int priority;       // Current Priority
    int counter;        // Program Counter
    int lower_bound;    // Lower Bound of the process’ space in the memory
    int upper_bound;    // Upper Bound of the process’ space in the memory
    int arrival_time;   // Arrival Time
    char filename[100]; // Filename
} PCB;

typedef struct
{
    int semaphore;
    int owner_pid;
    int blocked_pids[QUEUE_SIZE];
    int blocked_front, blocked_rear;
} Mutex;

Mutex userInputMutex;
Mutex userOutputMutex;
Mutex fileMutex;

// Global variables for memory and time
Memory memory;
int mem_start = 0;
int current_time = 0;
int time_quantum;

// Function to initialize memory
void initialize_memory()
{
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        memory.memory_blocks[i].name[0] = '\0';
        memory.memory_blocks[i].data[0] = '\0';
    }
}

// Function to initialize queues
void initialize_queues()
{
    for (int i = 0; i < MAX_PRIORITY; i++)
    {
        front[i] = rear[i] = -1;
    }
    blocked_front = blocked_rear = -1;
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

// Function to read data from a file
char *read_file(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: File not found\n");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = (char *)malloc(length + 1);
    if (buffer)
    {
        fread(buffer, 1, length, file);
        buffer[length] = '\0';
    }
    fclose(file);
    return buffer;
}

// Function to search for a variable in memory and return its data
char *search_variable(PCB *pcb, char *name)
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

// Function to assign value to a variable
void assign(char *arg1, char *arg2, char *arg3, PCB *pcb)
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
        if (strcmp(arg2, "readFile") == 0)
        {
            char *filename = search_variable(pcb, arg3);
            if (filename == NULL)
            {
                printf("Invalid filename %s \n", arg3);
                return;
            }
            char *output = read_file(filename);
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
void print(char *arg1, PCB *pcb)
{
    char *data = search_variable(pcb, arg1);
    if (data != NULL)
    {
        printf("%s\n", data);
    }
}

// Function to print a range of numbers
void print_from_to(char *arg1, char *arg2, PCB *pcb)
{
    char *from = search_variable(pcb, arg1);
    char *to = search_variable(pcb, arg2);
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
void write_file(char *arg1, char *arg2, PCB *pcb)
{
    char *filename = search_variable(pcb, arg1);
    char *data = search_variable(pcb, arg2);
    if (data != NULL && filename != NULL)
    {
        FILE *file = fopen(filename, "w");
        if (file != NULL)
        {
            fputs(data, file);
            fclose(file);
        }
    }
}

// Helper function to add process to the blocked queue
void enqueue_blocked(int pid, int queue[], int *front, int *rear)
{
    if ((*rear + 1) % QUEUE_SIZE == *front)
    {
        printf("Blocked queue is full\n");
        return;
    }
    if (*front == -1)
    {
        *front = 0;
    }
    *rear = (*rear + 1) % QUEUE_SIZE;
    queue[*rear] = pid;
}

// Helper function to remove process from blocked queue
int dequeue_blocked(int queue[], int *front, int *rear)
{
    if (*front == -1)
    {
        return -1;
    }
    int pid = queue[*front];
    if (*front == *rear)
    {
        *front = *rear = -1;
    }
    else
    {
        *front = (*front + 1) % QUEUE_SIZE;
    }
    return pid;
}

// Function to enqueue a process in the ready queue
void enqueue(int pid, int priority)
{
    if ((rear[priority] + 1) % QUEUE_SIZE == front[priority])
    {
        printf("Ready queue for priority %d is full\n", priority);
        return;
    }
    if (front[priority] == -1)
    {
        front[priority] = 0;
    }
    rear[priority] = (rear[priority] + 1) % QUEUE_SIZE;
    ready_queues[priority][rear[priority]] = pid;
}

// Function to dequeue a process from the ready queue based on priority
int dequeue()
{
    for (int i = 0; i < MAX_PRIORITY; i++)
    {
        if (front[i] != -1)
        {
            int pid = ready_queues[i][front[i]];
            if (front[i] == rear[i])
            {
                front[i] = rear[i] = -1;
            }
            else
            {
                front[i] = (front[i] + 1) % QUEUE_SIZE;
            }
            return pid;
        }
    }
    printf("Ready queues are empty\n");
    return -1;
}

// Function to decrement semaphore
void semWait(char *arg1, PCB *pcb)
{
    Mutex *mutex;
    if (strcmp(arg1, "userInput") == 0)
    {
        mutex = &userInputMutex;
    }
    else if (strcmp(arg1, "userOutput") == 0)
    {
        mutex = &userOutputMutex;
    }
    else if (strcmp(arg1, "file") == 0)
    {
        mutex = &fileMutex;
    }
    else
    {
        return;
    }

    if (mutex->semaphore == 1)
    {
        mutex->semaphore--;
        mutex->owner_pid = pcb->pid;
    }
    else
    {
        enqueue_blocked(pcb->pid, mutex->blocked_pids, &mutex->blocked_front, &mutex->blocked_rear);
        strcpy(pcb->state, "BLOCKED");
    }
}

// Function to increment semaphore
void semSignal(char *arg1, PCB *processes)
{
    Mutex *mutex;
    if (strcmp(arg1, "userInput") == 0)
    {
        mutex = &userInputMutex;
    }
    else if (strcmp(arg1, "userOutput") == 0)
    {
        mutex = &userOutputMutex;
    }
    else if (strcmp(arg1, "file") == 0)
    {
        mutex = &fileMutex;
    }
    else
    {
        return;
    }

    if (mutex->blocked_front != -1)
    {
        int next_pid = dequeue_blocked(mutex->blocked_pids, &mutex->blocked_front, &mutex->blocked_rear);
        PCB *process = &processes[next_pid - 1];
        strcpy(process->state, "READY");
        enqueue(next_pid, process->priority);
    }
    else
    {
        mutex->semaphore++;
    }
}

// Function to execute a process based on instructions in its program file
void execute_program(PCB *process, PCB *processes)
{
    char *file_content = read_file(process->filename);
    if (file_content == NULL)
    {
        return;
    }

    char *instruction = strtok(file_content, "\n");
    while (instruction != NULL && strcmp(process->state, "RUNNING") == 0 && process->counter < time_quantum)
    {
        char *args[4];
        char *token = strtok(instruction, " ");
        int i = 0;
        while (token != NULL && i < 4)
        {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        if (strcmp(args[0], "assign") == 0)
        {
            assign(args[1], args[2], args[3], process);
        }
        else if (strcmp(args[0], "print") == 0)
        {
            print(args[1], process);
        }
        else if (strcmp(args[0], "printFromTo") == 0)
        {
            print_from_to(args[1], args[2], process);
        }
        else if (strcmp(args[0], "writeFile") == 0)
        {
            write_file(args[1], args[2], process);
        }
        else if (strcmp(args[0], "semWait") == 0)
        {
            semWait(args[1], process);
        }
        else if (strcmp(args[0], "semSignal") == 0)
        {
            semSignal(args[1], processes);
        }

        process->counter++;
        instruction = strtok(NULL, "\n");
    }

    free(file_content);
    if (strcmp(process->state, "RUNNING") == 0)
    {
        strcpy(process->state, "READY");
    }
}

// Function to run the scheduler
void run_scheduler(PCB *processes, int num_processes)
{
    int completed = 0;
    while (completed < num_processes)
    {
        int pid = dequeue();
        if (pid == -1)
        {
            current_time++;
            continue;
        }

        PCB *process = &processes[pid - 1];
        if (strcmp(process->state, "NEW") == 0)
        {
            strcpy(process->state, "READY");
        }

        if (strcmp(process->state, "READY") == 0)
        {
            strcpy(process->state, "RUNNING");
            execute_program(process, processes);
            if (strcmp(process->state, "DEAD") != 0)
            {
                strcpy(process->state, "READY");
                enqueue(pid, process->priority);
            }
            else
            {
                completed++;
            }
        }

        // Unblock processes if they are no longer blocked
        for (int i = 0; i < num_processes; i++)
        {
            if (strcmp(processes[i].state, "BLOCKED") == 0)
            {
                int blocked_pid = dequeue_blocked(blocked_queue, &blocked_front, &blocked_rear);
                if (blocked_pid != -1)
                {
                    strcpy(processes[blocked_pid - 1].state, "READY");
                    enqueue(blocked_pid, processes[blocked_pid - 1].priority);
                }
            }
        }

        current_time++;
    }
}

// Function to initialize semaphores
void initialize_semaphores()
{
    userInputMutex.semaphore = 1;
    userInputMutex.owner_pid = -1;
    userInputMutex.blocked_front = userInputMutex.blocked_rear = -1;

    userOutputMutex.semaphore = 1;
    userOutputMutex.owner_pid = -1;
    userOutputMutex.blocked_front = userOutputMutex.blocked_rear = -1;

    fileMutex.semaphore = 1;
    fileMutex.owner_pid = -1;
    fileMutex.blocked_front = fileMutex.blocked_rear = -1;
}

int main()
{
    initialize_memory();
    userInputMutex.semaphore = 1;
    userOutputMutex.semaphore = 1;
    fileMutex.semaphore = 1;
    PCB processes[NUM_PROCESSES] = {
        {1, "READY", 0, 0, 0, 0, 0, "Program_1.txt"},
        {2, "READY", 0, 0, 0, 0, 0, "Program_2.txt"},
        {3, "READY", 0, 0, 0, 0, 0, "Program_3.txt"}};

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

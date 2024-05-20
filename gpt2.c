#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_PROCESSES 3
#define MEMORY_SIZE 60
#define QUEUE_SIZE 10


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
    int owner_id;
    int locked;
    } mutex;

mutex userInputMutex;
mutex userOutputMutex;
mutex fileMutex;

// Global variables for memory and time
Memory memory;
int mem_start = 0;
int current_time = 0;
int time_quantum;

int ready_queue[QUEUE_SIZE];
int front = -1, rear = -1;

// Function to enqueue a process in the ready queue
void enqueue(int pid)
    {
    if ((rear + 1) % QUEUE_SIZE == front)
        {
        printf("Ready queue is full\n");
        return;
        }
    if (front == -1)
        front = 0;
    rear = (rear + 1) % QUEUE_SIZE;
    ready_queue[rear] = pid;
    }

// Function to dequeue a process from the ready queue
int dequeue()
    {
    if (front == -1)
        {
        printf("Ready queue is empty\n");
        return -1;
        }
    int pid = ready_queue[front];
    if (front == rear)
        {
        front = rear = -1;
        }
    else
        {
        front = (front + 1) % QUEUE_SIZE;
        }
    return pid;
    }

void update_Memory_PCB(PCB* pcb)
    {
    for (int i = pcb->upper_bound + 3; i < pcb->upper_bound + 6; i++)
        {
        if (strcmp(memory.memory_blocks[i].name, "PID") == 0 && atoi(memory.memory_blocks[i].data) == pcb->pid) {
            strcpy(memory.memory_blocks[i + 1].data, pcb->state);
            sprintf(memory.memory_blocks[i + 2].data, "%d", pcb->priority);
            sprintf(memory.memory_blocks[i + 3].data, "%d", pcb->counter);
            sprintf(memory.memory_blocks[i + 4].data, "%d", pcb->lower_bound);
            sprintf(memory.memory_blocks[i + 5].data, "%d", pcb->upper_bound);
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

// Function to read data from a file
char* read_file(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: File not found\n");
        return NULL;
        }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)malloc(length);
    if (buffer) {
        fread(buffer, 1, length, file);
        }
    fclose(file);
    return buffer;
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
        if (strcmp(arg2, "readFile") == 0) {
            char* filename = search_variable(pcb, arg3);
            if (filename == NULL) {
                printf("Invalid filename %s \n", arg3);
                return;
                }
            char* output = read_file(filename);
            printf("Output: %s\n", output);
            if (output != NULL) {
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

// Function to decrement semaphore
void semWait(char* arg1, PCB* pcb) {
    if (strcmp(arg1, "userInput") == 0)
        {
        if (userInputMutex.semaphore >= 0)
            {
            printf("Process %d is waiting for userInput\n", pcb->pid);
            userInputMutex.owner_id = pcb->pid;

            userInputMutex.semaphore--;
            }
        else {
            strcpy(pcb->state, "BLOCKED");
            }
        }
    else if (strcmp(arg1, "userOutput") == 0)
        {
        if (userOutputMutex.semaphore >= 0)
            {
            printf("Process %d is waiting for userOutput\n", pcb->pid);
            userOutputMutex.owner_id = pcb->pid;
            userOutputMutex.semaphore--;
            }
        else {
            strcpy(pcb->state, "BLOCKED");
            }

        }
    else if (strcmp(arg1, "file") == 0)
        {
        if (fileMutex.semaphore >= 0)
            {
            printf("Process %d is waiting for file\n", pcb->pid);
            fileMutex.owner_id = pcb->pid;
            fileMutex.semaphore--;
            }
        else {
            strcpy(pcb->state, "BLOCKED");
            }
        }
    }

// Function to increment semaphore
void semSignal(char* arg1, PCB* pcb) {
    if (strcmp(arg1, "userInput") == 0)
        {
        userInputMutex.owner_id = 0;
        userInputMutex.semaphore++;
        }
    else if (strcmp(arg1, "userOutput") == 0)
        {
        userOutputMutex.owner_id = 0;
        userOutputMutex.semaphore++;
        }
    else if (strcmp(arg1, "file") == 0)
        {
        fileMutex.owner_id = 0;
        fileMutex.semaphore++;
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
        }
    else if (strcmp(command, "print") == 0)
        {
        print(arg1, pcb);
        }
    else if (strcmp(command, "printFromTo") == 0)
        {
        print_from_to(arg1, arg2, pcb);
        }
    else if (strcmp(command, "writeFile") == 0)
        {
        write_file(arg1, arg2, pcb);
        }
    else if (strcmp(command, "semWait") == 0)
        {
        semWait(arg1, pcb);
        }
    else if (strcmp(command, "semSignal") == 0)
        {
        semSignal(arg1, pcb);
        }
    else
        {
        printf("Invalid instruction\n");
        }
    }

// Function to write PCB to memory
void write_pcb_to_memory(PCB* pcb)
    {
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

// Function to load a program
void LoadProgram(char* filename, PCB* pcb)
    {
    FILE* file = fopen(filename, "r");
    if (file == NULL)
        {
        printf("Error: File not found\n");
        return;
        }
    strcpy(pcb->state, "READY");
    pcb->counter = 0;
    pcb->priority = 0;
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

// Function to execute a program in memory
void execute_program(PCB* pcb)
    {
    if (pcb == NULL)
        {
        printf("Error: Process not found\n");
        return;
        }

    char instruction[100];
    int time_spent = 0;
    for (int i = pcb->lower_bound + pcb->counter; i < pcb->upper_bound; i++)
        {
        if (time_spent >= time_quantum)
            break;
        if (strcmp(pcb->state, "BLOCKED") == 0)
            break;
        strcpy(instruction, memory.memory_blocks[i].data);
        printf("Executing instruction: %s\n", instruction);
        execute_instruction(instruction, pcb);
        pcb->counter++;
        time_spent++;
        }

    // Check if the program has finished execution
    if (pcb->counter >= (pcb->upper_bound - pcb->lower_bound))
        {
        strcpy(pcb->state, "DEAD");
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
            enqueue(processes[i].pid);
            LoadProgram(processes[i].filename, processes + i);
            }
        }
    }



// Function to run the scheduler
void run_scheduler(PCB processes[], int num_processes)
    {
    while (1)
        {
        add_arriving_processes(processes, num_processes);

        int pid = dequeue();
        if (pid == -1)
            {
            // Check if all processes are dead
            int all_dead = 1;
            for (int i = 0; i < num_processes; i++)
                {
                if (strcmp(processes[i].state, "DEAD") != 0)
                    {
                    all_dead = 0;
                    break;
                    }
                }
            if (all_dead)
                break;
            current_time++;
            continue;
            }

        PCB* process = &processes[pid - 1];

        if (strcmp(process->state, "READY") == 0 || strcmp(process->state, "RUNNING") == 0) {
            strcpy(process->state, "RUNNING");
            printf("\nExecuting process %d\n", process->pid);
            execute_program(process);

            if (strcmp(process->state, "DEAD") != 0)
                {
                strcpy(process->state, "READY");
                enqueue(process->pid);
                }
            }

        current_time++;
        }
    }

// Main function
int main()
    {
    // Initialize memory
    initialize_memory();
    userInputMutex.semaphore = 1;
    userOutputMutex.semaphore = 1;
    fileMutex.semaphore = 1;
    PCB processes[NUM_PROCESSES] = {
        {1, "READY", 0, 0, 0, 0, 0, "Program_1.txt"},
        {2, "READY", 0, 0, 0, 0, 0, "Program_2.txt"},
        {3, "READY", 0, 0, 0, 0, 0, "Program_3.txt"} };

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

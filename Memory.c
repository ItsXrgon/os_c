
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_PROCESSES 3
#define MEMORY_SIZE 60
#define QUEUE_SIZE 10
// Memory
typedef struct {
    char name[20];       // Name of the variable or instruction
    char data[200];      // Data of the variable or instruction
    } MemoryWord;

typedef struct {
    MemoryWord memory_blocks[MEMORY_SIZE];   // Memory divided into memory words
    } Memory;
typedef struct {
    int pid;             // Process ID
    char state[20];     // Process State
    int priority;        // Current Priority
    int counter;         // Program Counter
    int lower_bound;     // Lower Bound of the process’ space in the memory
    int upper_bound;     // Upper Bound of the process’ space in the memory
    int arrival_time;
    char filename[100];
    } PCB;

typedef struct {
    int semaphore;
    } mutex;

mutex userInputMutex;
mutex userOutputMutex;
mutex fileMutex;

int ready_queue[QUEUE_SIZE];
int front = -1, rear = -1;
PCB processes[NUM_PROCESSES];

// Global var for memory

Memory memory;
int mem_start = 0;

int current_time = 0;
int time_quantum;

// Function to initialize memory
void initialize_memory() {
    for (int i = 0; i < 60; i++) {
        memory.memory_blocks[i].name[0] = '\0';
        memory.memory_blocks[i].data[0] = '\0';
        }
    }

// Function to print memory 
void print_memory() {
    printf("Memory: \n");
    for (int i = 0; i < 60; i++) {
        if (memory.memory_blocks[i].name[0] != '\0') {
            printf("Name: %s  Value: %s\n", memory.memory_blocks[i].name, memory.memory_blocks[i].data);
            }
        }
    }
// Function to read data from a file
void read_file(const char* filename, char* buffer, size_t size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
        }
    size_t len = fread(buffer, 1, size, file);
    buffer[len] = '\0';
    fclose(file);
    }

// Function to assign value to a variable
void assign(char* arg1, char* arg2, PCB pcb) {
    if (strcmp(arg2, "input") == 0) {
        printf("Enter value for %s: ", arg1);
        scanf("%s", arg2);
        if (strcmp(arg1, "a") == 0) {
            strcpy(memory.memory_blocks[pcb.upper_bound].data, arg2);
            }
        else if (strcmp(arg1, "b") == 0) {
            strcpy(memory.memory_blocks[pcb.upper_bound + 1].data, arg2);
            }
        else if (strcmp(arg1, "c") == 0) {
            strcpy(memory.memory_blocks[pcb.upper_bound + 2].data, arg2);
            }
        }
    else {
        char command[20], var[20];
        static char file_name[100] = { 0 }, file_data[100] = { 0 }, buffer[100] = { 0 };
        sscanf(arg2, "%s %s", command, var);
        if (strcmp(command, "readFile") == 0) {
            read_file(file_name, buffer, sizeof(buffer));
            }
        }
    }

// Function to search for a variable in memory and return its data
char* search_variable(PCB pcb, char* name) {
    for (int i = pcb.upper_bound; i < pcb.upper_bound + 3; i++) {
        if (strcmp(memory.memory_blocks[i].name, name) == 0) {
            return memory.memory_blocks[i].data;
            }
        }
    return NULL;
    }

// Function to print a string
void print(char* arg1, PCB pcb) {
    char* data = search_variable(pcb, arg1);
    if (data != NULL) {
        printf("%s \n", data);
        }
    }

// Function to print a range of numbers
void print_from_to(char* arg1, char* arg2, PCB pcb) {

    char* from = search_variable(pcb, arg1);
    char* to = search_variable(pcb, arg2);
    if (from == NULL || to == NULL) {
        printf("Invalid range\n");
        return;
        }

    for (int i = atoi(from); i <= atoi(to); i++) {
        printf("%d\n", i);
        }
    }

//Function to write data to a file
void write_file(char* arg1, char* arg2, PCB pcb) {
    char* filename = search_variable(pcb, arg1);
    char* data = search_variable(pcb, arg2);
    if (data != NULL && filename != NULL) {
        FILE* file = fopen(filename, "w");
        if (file != NULL) {
            fputs(data, file);
            fclose(file);
            }
        }
    }



// Function to decrement semaphore
void semWait(char* arg1, PCB pcb) {

    }

// Function to increment semaphore
void semSignal(char* arg1, PCB pcb) {

    }

// Function to execute an instruction
void execute_instruction(char* instruction, PCB pcb) {
    char command[20], arg1[20], arg2[20];
    if (sscanf(instruction, "%s %s %s", command, arg1, arg2) < 1) {
        return;
        }

    if (strcmp(command, "assign") == 0) {
        assign(arg1, arg2, pcb);
        }
    else if (strcmp(command, "print") == 0) {
        print(arg1, pcb);
        }
    else if (strcmp(command, "printFromTo") == 0) {
        print_from_to(arg1, arg2, pcb);
        }
    else if (strcmp(command, "writefile") == 0) {
        write_file(arg1, arg2, pcb);
        }
    else if (strcmp(command, "semWait") == 0) {
        semWait(arg1, pcb);
        }
    else if (strcmp(command, "semSignal") == 0) {
        semSignal(arg1, pcb);
        }
    else {
        printf("Invalid instruction\n");
        }
    }

//Function to write pcb to memory
void write_pcb_to_memory(PCB pcb) {
    strcpy(memory.memory_blocks[mem_start].name, "PID");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb.pid);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "State");
    strcpy(memory.memory_blocks[mem_start].data, pcb.state);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "Priority");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb.priority);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "Counter");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb.counter);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "Lower_Bound");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb.lower_bound);
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "Upper_Bound");
    sprintf(memory.memory_blocks[mem_start].data, "%d", pcb.upper_bound);
    mem_start++;

    }

// Function to load a program
void LoadProgram(char* filename, PCB pcb) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: File not found\n");
        return;
        }
    strcpy(pcb.state, "Ready");
    pcb.counter = 0;
    pcb.priority = 0;
    pcb.lower_bound = mem_start;
    char instruction[100];
    int i = 0;
    while (fgets(instruction, 100, file)) {
        char instruction_name[100];
        sprintf(instruction_name, "Instruction_%d", i);
        strcpy(memory.memory_blocks[mem_start].name, instruction_name);
        strcpy(memory.memory_blocks[mem_start].data, instruction);
        mem_start++;
        i++;
        }
    fclose(file);
    pcb.upper_bound = mem_start;
    strcpy(memory.memory_blocks[mem_start].name, "a");
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "b");
    mem_start++;
    strcpy(memory.memory_blocks[mem_start].name, "c");
    mem_start++;

    // assign pcb to memory
    write_pcb_to_memory(pcb);
    }
PCB search_memory_for_pcb(int pid) {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (processes[i].pid == pid) {
            return processes[i];
            }
        }
    }
// Function to excute a program in memory
void execute_program(PCB pcb) {
    char instruction[100];
    int time_spent = 0;
    for (int i = pcb.lower_bound + pcb.counter;i < pcb.upper_bound; i++) {
        if (time_spent >= time_quantum) break;
        strcpy(instruction, memory.memory_blocks[i].data);
        execute_instruction(instruction, pcb);
        pcb.counter++;
        time_spent++;
        }

    }

void enqueue(int pid) {
    if ((rear + 1) % QUEUE_SIZE == front) {
        printf("Ready queue is full\n");
        return;
        }
    if (front == -1) front = 0;
    rear = (rear + 1) % QUEUE_SIZE;
    ready_queue[rear] = pid;
    }

void add_arriving_processes(PCB processes[], int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        if (strcmp(processes[i].state, "READY") == 0 && processes[i].arrival_time == current_time) {
            enqueue(processes[i].pid);
            LoadProgram(processes[i].filename, processes[i]);
            }
        }
    }

int dequeue() {
    if (front == -1) {
        printf("Ready queue is empty\n");
        return -1;
        }
    int pid = ready_queue[front];
    if (front == rear) {
        front = rear = -1;
        }
    else {
        front = (front + 1) % QUEUE_SIZE;
        }
    return pid;
    }



void run_scheduler(PCB processes[], int num_processes) {


    add_arriving_processes(processes, num_processes);

    int pid = dequeue();
    if (pid == -1) {
        // Check if all processes are dead
        int all_dead = 1;
        for (int i = 0; i < num_processes; i++) {
            if (strcmp(processes[i].state, "DEAD") != 0) {
                all_dead = 0;
                break;
                }
            }
        if (all_dead) return;
        current_time++;
        }

    PCB pcb = search_memory_for_pcb(pid);

    if (strcmp(pcb.state, "READY") == 0 || strcmp(pcb.state, "RUNNING") == 0) {
        strcpy(pcb.state, "RUNNING");
        printf("\nExecuting process %d\n", pcb.pid);
        execute_program(pcb);

        if (strcmp(pcb.state, "DEAD") != 0) {
            strcpy(pcb.state, "READY");
            enqueue(pcb.pid);
            }
        }

    current_time++;


    }

// Main function
int main() {
    // Initialize memory
    initialize_memory();
    userInputMutex.semaphore = 1;
    userOutputMutex.semaphore = 1;
    fileMutex.semaphore = 1;



    for (int i = 1; i < 4;i++) {
        PCB pcb;
        pcb.pid = i;
        pcb.arrival_time = 0;
        pcb.counter = 0;
        pcb.priority = 0;
        pcb.lower_bound = 0;
        pcb.upper_bound = 0;
        strcpy(pcb.state, "READY");
        sprintf(pcb.filename, "Program_%d.txt", i);
        processes[i - 1] = pcb;
        }

    printf("Enter the time quantum: ");
    scanf("%d", &time_quantum);

    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("Enter arrival time for process %d: ", i + 1);
        scanf("%d", &((processes + i)->arrival_time));
        }

    run_scheduler(processes, NUM_PROCESSES);

    print_memory();

    return 0;
    }
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 60
#define MAX_INSTRUCTIONS 20
#define MAX_PROCESSES 3

typedef struct Memory {
    char data[MEMORY_SIZE][20]; // each word can store a name and data pair
    } Memory;

Memory memory;

typedef struct PCB {
    int process_id;
    char state[10];
    int priority;
    int program_counter;
    int memory_lower_bound;
    int memory_upper_bound;
    char variables[3][2][20]; // to store variable names and values
    } PCB;

PCB process_list[MAX_PROCESSES]; // list of all processes
int process_count = 0;

typedef struct Mutex {
    int locked;
    int owner_process_id;
    int blocked_queue[10]; // simple queue to manage blocked processes
    int blocked_count;
    } Mutex;

Mutex file_mutex = { 0, -1, {0}, 0 };
Mutex input_mutex = { 0, -1, {0}, 0 };
Mutex output_mutex = { 0, -1, {0}, 0 };

void load_program(const char* filename, int process_id) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        exit(1);
        }

    PCB* pcb = &process_list[process_count++];
    pcb->process_id = process_id;
    strcpy(pcb->state, "Ready");
    pcb->priority = 0; // default priority
    pcb->program_counter = 0;
    pcb->memory_lower_bound = process_id * MAX_INSTRUCTIONS;
    pcb->memory_upper_bound = pcb->memory_lower_bound + MAX_INSTRUCTIONS - 1;

    int instruction_index = pcb->memory_lower_bound;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // remove newline character
        strcpy(memory.data[instruction_index++], line);
        }

    fclose(file);
    }

void semWait(Mutex* mutex, PCB* pcb) {
    if (mutex->locked) {
        // Add the process to the blocked queue
        mutex->blocked_queue[mutex->blocked_count++] = pcb->process_id;
        strcpy(pcb->state, "Blocked");
        }
    else {
        mutex->locked = 1;
        mutex->owner_process_id = pcb->process_id;
        }
    }

void semSignal(Mutex* mutex, PCB* pcb) {
    if (mutex->owner_process_id == pcb->process_id) {
        if (mutex->blocked_count > 0) {
            int next_process_id = mutex->blocked_queue[0];
            // Remove the first process from the blocked queue
            for (int i = 1; i < mutex->blocked_count; ++i) {
                mutex->blocked_queue[i - 1] = mutex->blocked_queue[i];
                }
            mutex->blocked_count--;
            // Unblock the process
            PCB* next_pcb = &process_list[next_process_id - 1];
            strcpy(next_pcb->state, "Ready");
            }
        else {
            mutex->locked = 0;
            mutex->owner_process_id = -1;
            }
        }
    }

PCB* get_pcb_by_id(int process_id) {
    for (int i = 0; i < process_count; ++i) {
        if (process_list[i].process_id == process_id) {
            return &process_list[i];
            }
        }
    return NULL;
    }

void execute_instruction(char* instruction, PCB* pcb) {
    char command[20];
    sscanf(instruction, "%s", command);

    if (strcmp(command, "print") == 0) {
        char var_name[20];
        sscanf(instruction, "print %s", var_name);
        for (int i = 0; i < 3; ++i) {
            if (strcmp(pcb->variables[i][0], var_name) == 0) {
                printf("%s\n", pcb->variables[i][1]);
                break;
                }
            }
        }
    else if (strcmp(command, "assign") == 0) {
        char var_name[20], value[20];
        sscanf(instruction, "assign %s %s", var_name, value);
        if (strcmp(value, "input") == 0) {
            printf("Please enter a value for %s: ", var_name);
            scanf("%s", value);
            }
        else if (strcmp(value, "readFile") == 0) {
            // Simulating reading a file
            char file_var[20];
            sscanf(instruction, "assign %s readFile %s", var_name, file_var);
            strcpy(value, "file_content"); // Example content from file
            }
        for (int i = 0; i < 3; ++i) {
            if (pcb->variables[i][0][0] == '\0' || strcmp(pcb->variables[i][0], var_name) == 0) {
                strcpy(pcb->variables[i][0], var_name);
                strcpy(pcb->variables[i][1], value);
                break;
                }
            }
        }
    else if (strcmp(command, "writeFile") == 0) {
        // Simulating writing to a file
        }
    else if (strcmp(command, "readFile") == 0) {
        // Simulating reading from a file
        }
    else if (strcmp(command, "printFromTo") == 0) {
        char var_name1[20], var_name2[20];
        sscanf(instruction, "printFromTo %s %s", var_name1, var_name2);
        int start, end;
        for (int i = 0; i < 3; ++i) {
            if (strcmp(pcb->variables[i][0], var_name1) == 0) {
                start = atoi(pcb->variables[i][1]);
                }
            if (strcmp(pcb->variables[i][0], var_name2) == 0) {
                end = atoi(pcb->variables[i][1]);
                }
            }
        for (int i = start; i <= end; ++i) {
            printf("%d\n", i);
            }
        }
    else if (strcmp(command, "semWait") == 0) {
        char resource[20];
        sscanf(instruction, "semWait %s", resource);
        if (strcmp(resource, "userInput") == 0) {
            semWait(&input_mutex, pcb);
            }
        else if (strcmp(resource, "userOutput") == 0) {
            semWait(&output_mutex, pcb);
            }
        else if (strcmp(resource, "file") == 0) {
            semWait(&file_mutex, pcb);
            }
        }
    else if (strcmp(command, "semSignal") == 0) {
        char resource[20];
        sscanf(instruction, "semSignal %s", resource);
        if (strcmp(resource, "userInput") == 0) {
            semSignal(&input_mutex, pcb);
            }
        else if (strcmp(resource, "userOutput") == 0) {
            semSignal(&output_mutex, pcb);
            }
        else if (strcmp(resource, "file") == 0) {
            semSignal(&file_mutex, pcb);
            }
        }
    }

void run_process(PCB* pcb) {
    while (strcmp(pcb->state, "Ready") == 0) {
        char* instruction = memory.data[pcb->memory_lower_bound + pcb->program_counter];
        execute_instruction(instruction, pcb);
        pcb->program_counter++;
        if (pcb->program_counter > pcb->memory_upper_bound) {
            strcpy(pcb->state, "Finished");
            }
        }
    }

int main() {
    memory = (Memory){ {0} };
    load_program("Program_1.txt", 1);
    load_program("Program_2.txt", 2);
    load_program("Program_3.txt", 3);

    for (int i = 0; i < process_count; i++) {
        PCB* pcb = &process_list[i];
        run_process(pcb);
        printf("Process ID: %d\n", pcb->process_id);
        for (int j = 0; j <= MEMORY_SIZE; j++) {
            if (strlen(memory.data[j]) > 0) {
                printf("%s\n", memory.data[j]);
                }
            }
        printf("\n");
        }

    return 0;
    }

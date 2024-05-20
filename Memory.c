
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Memory
typedef struct {
    char name[20];       // Name of the variable or instruction
    char data[200];      // Data of the variable or instruction
    } MemoryWord;

typedef struct {
    MemoryWord memory_blocks[60];   // Memory divided into memory words
    } Memory;


// Global var for memory

Memory memory;

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

// Function to assign value to a variable
void assign_memory(char* variable, char* value) {
    for (int i = 0; i < 60; i++) {
        if (strcmp(memory.memory_blocks[i].name, variable)) {
            strcpy(memory.memory_blocks[i].data, value);
            break;
            }
        }
    }

// Function to search for a variable in memory and return its data
char* search_memory(char* name) {
    for (int i = 0; i < 60; i++) {
        if (strcmp(memory.memory_blocks[i].name, name) == 0) {
            return memory.memory_blocks[i].data;
            }
        }
    return NULL;
    }

// Function to print a string
void print(char* arg1) {
    char* data = search_memory(arg1);
    if (data != NULL) {
        printf("%s \n", data);
        }
    }

// Function to print a range of numbers
void print_from_to(int from, int to) {
    for (int i = from; i <= to; i++) {
        printf("%d\n", i);
        }
    }

//Function to write data to a file
void write_file(char* arg1, char* arg2) {
    char* data = search_memory(arg1);
    if (data != NULL) {
        FILE* file = fopen(arg2, "w");
        if (file != NULL) {
            fputs(data, file);
            fclose(file);
            }
        }
    }

// Function to read data from a file
void read_file(char* arg1, char* arg2) {
    FILE* file = fopen(arg1, "r");
    if (file != NULL) {
        char data[100];
        while (fgets(data, 100, file)) {
            assign_memory(arg2, data);
            }
        fclose(file);
        }
    }

// Function to decrement semaphore
void semWait(char* arg1) {

    }

// Function to increment semaphore
void semSignal(char* arg1) {

    }

// Function to execute an instruction
void execute_instruction(char* instruction) {
    char command[20], arg1[20], arg2[20];
    if (sscanf(instruction, "%s %s %s", command, arg1, arg2) < 1) {
        return;
        }

    if (strcmp(command, "assign") == 0) {
        assign_memory(arg1, arg2);
        }
    else if (strcmp(command, "print") == 0) {
        print(arg1);
        }
    else if (strcmp(command, "printFromTo") == 0) {
        print_from_to(atoi(arg1), atoi(arg2));
        }
    else if (strcmp(command, "writefile") == 0) {
        write_file(arg1, arg2);
        }
    else if (strcmp(command, "readfile") == 0) {
        read_file(arg1, arg2);
        }
    else if (strcmp(command, "printmemory") == 0) {
        print_memory();
        }
    else if (strcmp(command, "semWait") == 0) {
        semWait(arg1);
        }
    else if (strcmp(command, "semSignal") == 0) {
        semSignal(arg1);
        }
    else {
        printf("Invalid instruction\n");
        }
    }

// Function to load a program
void LoadProgram(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: File not found\n");
        return;
        }
    char instruction[100];
    int i = 0;
    while (fgets(instruction, 100, file)) {
        char instruction_name[100];
        sprintf(instruction_name, "Instruction_%d", i);
        strcpy(memory.memory_blocks[i].name, instruction_name);
        strcpy(memory.memory_blocks[i].data, instruction);
        i++;
        }
    fclose(file);
    }

// Function to excute a program in memory
void execute_program() {
    for (int i = 0; i < 60; i++) {
        if (memory.memory_blocks[i].name[0] != '\0') {
            execute_instruction(memory.memory_blocks[i].data);
            }
        }
    }

// Main function
int main() {
    // Initialize memory
    initialize_memory();

    // Sample instructions
    LoadProgram("Program_1.txt");

    // Print memory
    print_memory();

    // Execute program
    execute_program();


    return 0;
    }
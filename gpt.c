#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 60
#define MAX_INSTRUCTIONS 20
#define MAX_PROCESSES 3
#define QUEUE_SIZE 10

// Process Control Block (PCB)
typedef struct {
    int pid;             // Process ID
    char state[10];      // Process State
    int priority;        // Current Priority
    int counter;         // Program Counter
    int lower_bound;     // Lower Bound of the process’ space in the memory
    int upper_bound;     // Upper Bound of the process’ space in the memory
    } PCB;

// Memory
typedef struct {
    char name[20];       // Name of the variable or instruction
    char data[100];      // Data of the variable or instruction
    } MemoryWord;

typedef struct {
    MemoryWord memory_blocks[60];   // Memory divided into memory words
    } Memory;

// Process
typedef struct {
    int id;              // Process ID
    char filename[100];  // Filename
    PCB pcb;             // PCB of the process
    } Process;

// Queue
typedef struct {
    Process* processes[60]; // Array of pointers to processes
    int front, rear, size;
    unsigned capacity;
    } Queue;

typedef struct {
    int locked;
    int owner_process_id;
    int blocked_queue[10]; // simple queue to manage blocked processes
    int blocked_count;
    } Mutex;

// Global variable for the blocked queue
Queue blockedQueue;

// Global variable for the ready queue
Queue readyQueue;

// Global variable to represent the current process
PCB* currentProcess;

Mutex userInputMutex;
Mutex userOutputMutex;
Mutex fileMutex;

Memory memory;

// Helper function to find a mutex based on the resource name
Mutex* findMutex(char* resource) {
    // Logic to find and return the mutex based on the resource name
    if (strcmp(resource, "userInput") == 0) {
        return &userInputMutex;
        }
    else if (strcmp(resource, "userOutput") == 0) {
        return &userOutputMutex;
        }
    else if (strcmp(resource, "file") == 0) {
        return &fileMutex;
        }
    }

// Helper function to add a process to the blocked queue
void addToBlockedQueue(PCB* process) {
    // Add the process to the blocked queue

    blockedQueue.processes[blockedQueue.rear] = process;
    blockedQueue.rear = (blockedQueue.rear + 1) % blockedQueue.capacity;
    blockedQueue.size++;

    }

// Helper function to remove the highest priority process from the ready queue
PCB* removeFromReadyQueue() {
    // Logic to remove and return the highest priority process from the ready queue

    PCB* process = readyQueue.processes[readyQueue.front];
    readyQueue.front = (readyQueue.front + 1) % readyQueue.capacity;
    readyQueue.size--;
    return process;
    }
// Helper function to check if a queue is empty
int isQueueEmpty(Queue queue) {
    // Logic to check if the queue is empty and return a boolean value

    return queue.size == 0;
    }

// Helper function to handle context switching to the next ready process
void contextSwitch() {
    // Logic to perform a context switch to the next ready process

    if (isQueueEmpty(readyQueue)) {
        // No ready processes, continue with the current process
        return;
        }
    else {
        // Get the next process from the ready queue
        PCB* nextProcess = removeFromReadyQueue();
        // Switch to the next process
        currentProcess = nextProcess;
        }
    }

// Helper function to remove the highest priority process from the blocked queue
PCB* removeFromBlockedQueue() {
    // Logic to remove and return the highest priority process from the blocked queue

    PCB* process = blockedQueue.processes[blockedQueue.front];
    blockedQueue.front = (blockedQueue.front + 1) % blockedQueue.capacity;
    blockedQueue.size--;
    return process;
    }

// Helper function to add a process to the ready queue
void addToReadyQueue(PCB* process) {
    // Add the process to the ready queue

    readyQueue.processes[readyQueue.rear] = process;
    readyQueue.rear = (readyQueue.rear + 1) % readyQueue.capacity;
    readyQueue.size++;
    }


/*
This function is used to acquire a resource.
If the resource is being used by another process,
this process should be blocked and added to the blocked queue.
*/
void sem_Wait(char* resource) {
    Mutex* mutex = findMutex(resource); // Assume there's a function to find the mutex based on the resource name
    if (mutex->locked == 1) {
        // Add the process to the blocked queue
        addToBlockedQueue(currentProcess); // Assume currentProcess is the PCB of the currently running process
        // Implement logic to switch to the next ready process
        contextSwitch();
        }
    else {
        mutex->locked = 1;
        }
    }

/*
This function is used to release a resource.
If there are processes waiting for this resource,
the one with the highest priority should be unblocked and added to the ready queue.
*/
void sem_Signal(char* resource) {
    Mutex* mutex = findMutex(resource); // Assume there's a function to find the mutex based on the resource name
    if (isQueueEmpty(blockedQueue)) {
        mutex->locked = 0;
        }
    else {
        PCB* highestPriorityProcess = removeFromBlockedQueue(); // Remove the highest priority process from the blocked queue
        addToReadyQueue(highestPriorityProcess); // Add the process to ready queue
        }
    }

/*
*
This function prints the value of the variable on the screen.
Before printing, the function should use
 semWait to acquire the userOutput resource and semSignal to release it after printing.
*

@param variable: the variable to be printed
*/
void print(char* variable) {
    semWait("userOutput");
    printf("%s\n", variable);
    semSignal("userOutput");
    }



/*
*This function assigns a value to a variable.
If the value is input, it should print "Please enter a value" to the screen,
 then take the value as an input from the user.

@param variable: the variable to be assigned
@param value: the value to be assigned

*/
void assign(char* variable, char* value) {
    if (strcmp(value, "input") == 0) {
        semWait("userInput");
        printf("Please enter a value: ");
        scanf("%s", variable);
        semSignal("userInput");
        }
    else {
        strcpy(variable, value);
        }
    }

/*
This function writes data to a file.
 It should use semWait to acquire the file resource before writing and semSignal to release it after writing.

@param filename: the name of the file
@param data: the data to be written to the file
*/
void writeFile(char* filename, char* data) {
    semWait("file");
    FILE* file = fopen(filename, "w");
    if (file != NULL) {
        fputs(data, file);
        fclose(file);
        }
    semSignal("file");
    }

/*
This function reads data from a file and prints it on the screen.
It should use semWait to acquire the file and userOutput resources before reading and printing,
and semSignal to release them after.

@param filename: the name of the file
*/
void readFile(char* filename) {
    semWait("file");
    semWait("userOutput");
    char c;
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        while ((c = getc(file)) != EOF)
            putchar(c);
        fclose(file);
        }
    semSignal("userOutput");
    semSignal("file");
    }


/*
This function prints all numbers between two numbers, inclusive.
It should use semWait to acquire the userOutput resource before printing and semSignal to release it after printing.

@param from: the starting number
@param to: the ending number
*/
void printFromTo(int from, int to) {
    semWait("userOutput");
    for (int i = from; i <= to; i++) {
        printf("%d\n", i);
        }
    semSignal("userOutput");
    }


int main() {

    return 0;
    }
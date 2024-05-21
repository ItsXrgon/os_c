#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

#define MEMORY_SIZE 60
typedef struct
    {
    int pid;            // Process ID
    char state[20];     // Process State
    int counter;        // Program Counter
    int lower_bound;    // Lower Bound of the process’ space in the memory
    int upper_bound;    // Upper Bound of the process’ space in the memory
    int arrival_time;   // Arrival Time
    char filename[100]; // Filename
    } PCB;

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
    int owner_id;
    int blocked_queue[10];
    int locked;
    int front;
    int rear;
    } mutex;
#endif // STRUCTS_H_INCLUDED
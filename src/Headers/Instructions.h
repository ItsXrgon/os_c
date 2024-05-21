#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H


#include "structs.h"


Mutex* chooseMutex(char* arg1);

void initMutex(Mutex* m);

char* read_file(char* filename);

void assign(char* arg1, char* arg2, char* arg3, PCB* pcb);

void print(char* arg1, PCB* pcb);

void print_from_to(char* arg1, char* arg2, PCB* pcb);

void write_file(char* arg1, char* arg2, PCB* pcb);

void semWait(Mutex* m, PCB* pcb);

void semSignal(Mutex* m, PCB* pcb);

void execute_instruction(char* instruction, PCB* pcb);

#endif
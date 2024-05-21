#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H


#include "structs.h"


mutex* chooseMutex(char* arg1);

void initialize_mutex();

char* read_file(char* filename);

void assign(char* arg1, char* arg2, char* arg3, PCB* pcb);

void print(char* arg1, PCB* pcb);

void print_from_to(char* arg1, char* arg2, PCB* pcb);

void write_file(char* arg1, char* arg2, PCB* pcb);

void semWait(mutex* m, PCB* pcb);

void semSignal(mutex* m, PCB* pcb);

void execute_instruction(char* instruction, PCB* pcb);

#endif
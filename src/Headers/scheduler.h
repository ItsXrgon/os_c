#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "structs.h"

void update_PCB(PCB* pcb);

void execute_program(PCB* pcb);

void add_arriving_processes(PCB processes[], int num_processes);

void schedule(PCB processes[], int num_processes);
void updatePCB(PCB* pcb);
int All_Terminated(PCB processes[], int num_processes);

#endif
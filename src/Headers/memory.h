#ifndef MEMORY_H
#define MEMORY_H


void update_Memory_PCB(PCB* pcb);
void initialize_memory();
void print_memory();
char* search_variable(PCB* pcb, char* name);
void write_pcb_to_memory(PCB* pcb);

void LoadProgram(char* filename, PCB* pcb);

#endif
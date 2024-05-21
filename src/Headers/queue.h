#ifndef QUEUE_H
#define QUEUE_H

#include "structs.h"

void initQueue(Queue* q);

void enqueue(Queue* q, PCB* pcb);

PCB* dequeue(Queue* q);

int isQueueEmpty(Queue* q);


#endif
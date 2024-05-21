#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "../Headers/structs.h"
#define queueSize 10

void initQueue(Queue* q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
    }

void enqueue(Queue* q, PCB* pcb) {
    if (q->count < queueSize) {
        q->rear = (q->rear + 1) % queueSize;
        q->queue[q->rear] = pcb;
        q->count++;
        printf("Enqueued process with PID: %d\n", pcb->pid);
        }
    else {
        printf("Queue is full\n");
        }
    }

PCB* dequeue(Queue* q) {
    if (q->count > 0) {
        PCB* pcb = q->queue[q->front];
        q->front = (q->front + 1) % queueSize;
        q->count--;
        printf("Dequeued process with PID: %d\n", pcb->pid);
        return pcb;
        }
    else {
        printf("Queue is empty\n");
        return NULL;
        }
    }

int isQueueEmpty(Queue* q) {
    return q->count == 0;
    }
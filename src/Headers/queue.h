#ifndef QUEUE_H
#define QUEUE_H

int isFull(int* front, int* rear);
int isEmpty(int* front, int* rear);
void enqueue(int* value, int myQueue[], int* front, int* rear);
int dequeue(int myQueue[], int* front, int* rear);
void display(int myQueue[], int* front, int* rear);
int ifExists(int myQueue[], int* front, int* rear, int value);
#endif
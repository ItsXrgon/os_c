#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

int isFull(int front, int rear);
int isEmpty(int front, int rear);
void enqueue(int value, int myQueue[], int front, int rear);
int dequeue(int myQueue[], int front, int rear);
void display(int myQueue[], int front, int rear);
#endif
#include<stdio.h>
#define queueSize 10

// Function to check if the ready queue is full
int isFull(int front, int rear)
    {
    return (rear == queueSize - 1 && front == 0) || (rear + 1 == front);
    }
// Function to check if the ready queue is empty
int isEmpty(int front, int rear) {
    return front == -1 && rear == -1;
    }

void enqueue(int value, int myQueue[], int front, int rear)
    {
    if (isFull(front, rear)) {
        printf("queue is full\n");
        return;
        }
    if (front == -1) {
        front = 0;
        }
    rear = (rear + 1) % queueSize;
    myQueue[rear] = value;
    printf("enqueued %d\n", value);
    }

int dequeue(int myQueue[], int front, int rear)
    {
    if (isEmpty(front, rear)) {
        printf("queue is empty\n");
        return -1;
        }
    int value = myQueue[front];
    if (front == rear) {
        front = rear = -1;
        }
    else {
        front = (front + 1) % queueSize;
        }
    printf("dequeued %d\n", value);
    return value;
    }

void display(int myQueue[], int front, int rear)
    {
    int i;

    for (i = front; i <= rear; i++)
        printf("%d\n", myQueue[i]);
    }
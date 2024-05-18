#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 60
#define PROCESS_VARS_SIZE 3
#define NUM_PROCESSES 3

enum State { READY, BLOCKED, RUNNING, DEAD };

struct Process {
    int pid;
    enum State state;
    int priority;
    int program_counter;
    int memory_boundaries[2];
    char variables[PROCESS_VARS_SIZE][100];
};

struct Memory {
    char memory_words[MEMORY_SIZE][100];
    struct Process process_table[NUM_PROCESSES];
};

struct Scheduler {
    struct Process* queues[4][NUM_PROCESSES];
    int queue_sizes[4];
    struct Process* blocked_queue[NUM_PROCESSES];
    int blocked_queue_size;
};

int semaphore_userInput = 1, semaphore_userOutput = 1, semaphore_file = 1;

void sem_wait(int* semaphore) {
    while (*semaphore <= 0) {
        // Busy-wait
    }
    (*semaphore)--;
}

void sem_signal(int* semaphore) {
    (*semaphore)++;
}

void write_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    fprintf(file, "%s", content);
    fclose(file);
}

void read_file(const char* filename, char* buffer, size_t size) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    size_t len = fread(buffer, 1, size, file);
    buffer[len] = '\0';
    fclose(file);
}

void print_from_to(int from, int to) {
    for (int i = from; i <= to; i++) {
        printf("%d\n", i);
    }
}

void assign(char* variable, char* value) {
    printf("Enter value for %s: ", variable);
    scanf("%s", value);
}

void print(char* str) {
    printf("%s\n", str);
}

void execute_program_1() {
    sem_wait(&semaphore_userInput);
    int x, y;
    printf("Enter the first number: ");
    scanf("%d", &x);
    printf("Enter the second number: ");
    scanf("%d", &y);
    sem_signal(&semaphore_userInput);

    sem_wait(&semaphore_userOutput);
    print_from_to(x, y);
    sem_signal(&semaphore_userOutput);
}

void execute_program_2() {
    sem_wait(&semaphore_userInput);
    char filename[100];
    char data[100];
    printf("Enter the filename: ");
    scanf("%s", filename);
    printf("Enter the data to write to the file: ");
    scanf(" %[^\n]", data);  // This allows reading a string with spaces
    sem_signal(&semaphore_userInput);

    sem_wait(&semaphore_file);
    write_file(filename, data);
    sem_signal(&semaphore_file);
}

void execute_program_3() {
    sem_wait(&semaphore_userInput);
    char filename[100];
    printf("Enter the filename: ");
    scanf("%s", filename);
    sem_signal(&semaphore_userInput);

    sem_wait(&semaphore_file);
    char file_content[100];
    read_file(filename, file_content, sizeof(file_content));
    sem_signal(&semaphore_file);

    sem_wait(&semaphore_userOutput);
    printf("File contents:\n%s\n", file_content);
    sem_signal(&semaphore_userOutput);
}

void add_to_queue(struct Process* process, struct Scheduler* scheduler) {
    int priority = process->priority;
    scheduler->queues[priority][scheduler->queue_sizes[priority]++] = process;
}

void run_scheduler(struct Scheduler* scheduler) {
    while (1) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < scheduler->queue_sizes[i]; j++) {
                struct Process* process = scheduler->queues[i][j];
                process->state = RUNNING;
                switch (process->pid) {
                    case 1:
                        execute_program_1();
                        break;
                    case 2:
                        execute_program_2();
                        break;
                    case 3:
                        execute_program_3();
                        break;
                }
                process->state = READY;
            }
        }
    }
}

int main() {
    struct Memory memory;
    struct Scheduler scheduler;

    memset(&scheduler, 0, sizeof(scheduler));

    struct Process process1 = {1, READY, 1, 0, {0, 3}, {"a", "b", "c"}};
    struct Process process2 = {2, READY, 1, 0, {4, 7}, {"a", "b", "c"}};
    struct Process process3 = {3, READY, 1, 0, {8, 11}, {"a", "b", "c"}};

    add_to_queue(&process1, &scheduler);
    add_to_queue(&process2, &scheduler);
    add_to_queue(&process3, &scheduler);

    run_scheduler(&scheduler);

    return 0;
}

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
    char program_file[20];
    int variables[PROCESS_VARS_SIZE];
};

struct Scheduler {
    struct Process* queues[4][NUM_PROCESSES];
};

int userInput = 1;
int userOutput = 1;
int file = 1;

void sem_wait(int* semaphore) {
    while (*semaphore <= 0) {
        // Busy wait
    }
    (*semaphore)--;
}

void sem_signal(int* semaphore) {
    (*semaphore)++;
}

void write_file(char* filename, char* content) {
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

void execute_instruction(char* instruction) {
    char command[20], arg1[20], arg2[20];
    static char file_name[100] = {0}, file_data[100] = {0}, buffer[100] = {0};

    if (sscanf(instruction, "%s %s %s", command, arg1, arg2) < 1) {
        return;
    }

    if (strcmp(command, "semWait") == 0) {
        if (strcmp(arg1, "userInput") == 0) sem_wait(&userInput);
        if (strcmp(arg1, "userOutput") == 0) sem_wait(&userOutput);
        if (strcmp(arg1, "file") == 0) sem_wait(&file);
    } else if (strcmp(command, "semSignal") == 0) {
        if (strcmp(arg1, "userInput") == 0) sem_signal(&userInput);
        if (strcmp(arg1, "userOutput") == 0) sem_signal(&userOutput);
        if (strcmp(arg1, "file") == 0) sem_signal(&file);
    } else if (strcmp(command, "assign") == 0) {
        if (strcmp(arg1, "a") == 0) {
            if (strcmp(arg2, "input") == 0) {
                assign("a", file_name);
            } else if (strcmp(arg2, "readFile") == 0) {
                read_file(file_name, buffer , sizeof(buffer));
            }
        }
        if (strcmp(arg1, "b") == 0) {
            if (strcmp(arg2, "input") == 0) {
                assign("b", file_data);
            }
            else if (strcmp(arg2, "readFile") == 0) {
                read_file(file_name, buffer , sizeof(buffer));
            }
        }
    } else if (strcmp(command, "writeFile") == 0) {
        write_file(file_name, file_data);
    } else if (strcmp(command, "readFile") == 0) {
        read_file(file_name, buffer , sizeof(buffer));
    } else if (strcmp(command, "printFromTo") == 0) {
        int from = atoi(file_name); 
        int to = atoi(file_data);
        print_from_to(from, to);
    } else if (strcmp(command, "print") == 0) {
        print(buffer);
    }
}

void execute_program(struct Process* process) {
    FILE* file = fopen(process->program_file, "r");
    if (file == NULL) {
        printf("Error opening program file %s\n", process->program_file);
        return;
    }

    char instruction[100];
    printf("\nExecuting process %d\n", process->pid); // Start of process execution
    while (fgets(instruction, sizeof(instruction), file)) {
        printf("Executing instruction: %s", instruction);
        execute_instruction(instruction);
    }
    

    fclose(file);
}

void run_scheduler(struct Scheduler* scheduler) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < NUM_PROCESSES; j++) {
            if (scheduler->queues[i][j] != NULL) {
                struct Process* process = scheduler->queues[i][j];
                if (process->state == READY) {
                    process->state = RUNNING;
                    execute_program(process);
                    process->state = READY;
                }
            }
        }
    }
}

int main() {
    struct Scheduler scheduler = { 0 };

    struct Process process1 = { 1, READY, 2, 0, "Program_1.txt" };
    struct Process process2 = { 2, READY, 3, 0, "Program_2.txt" };
    struct Process process3 = { 3, READY, 4, 0, "Program_3.txt" };

    scheduler.queues[1][0] = &process1;
    scheduler.queues[2][0] = &process2;
    scheduler.queues[3][0] = &process3;

    run_scheduler(&scheduler);

    return 0;
}

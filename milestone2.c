#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 60
#define NUM_PROCESSES 3
#define QUEUE_SIZE 10

int ready_queue[QUEUE_SIZE];
int front = -1, rear = -1;

enum State { READY, RUNNING, DEAD };

typedef struct {
    int pid;
    enum State state;
    int program_counter;
    char program_file[20];
    int variables[3];
    int arrival_time;
    int mem_start;
} Process;

typedef struct {
    char name[20];
    int value;
} MemoryElement;

typedef struct {
    int semaphore;
} mutex;

mutex userInputMutex;
mutex userOutputMutex;
mutex fileMutex;

MemoryElement memory[MEMORY_SIZE];

int current_time = 0;
int time_quantum;

// Program Syntax
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

void sem_wait(int* semaphore) {
    while (*semaphore <= 0) {
        // Busy wait
    }
    (*semaphore)--;
}

void sem_signal(int* semaphore) {
    (*semaphore)++;
}

void print_memory() {
    printf("Memory: \n");
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].name[0] != '\0') {
            printf("%s: %d\n", memory[i].name, memory[i].value);
        }
    }
}

void execute_instruction(Process* process, char* instruction) {
    char command[20], arg1[20], arg2[20];
    static char file_name[100] = { 0 }, file_data[100] = { 0 }, buffer[100] = { 0 };

    if (sscanf(instruction, "%s %s %s", command, arg1, arg2) < 1) {
        return;
    }

    printf("Executing instruction: %s\n", instruction);

    if (strcmp(command, "semWait") == 0) {
        if (strcmp(arg1, "userInput") == 0) sem_wait(&userInputMutex.semaphore);
        if (strcmp(arg1, "userOutput") == 0) sem_wait(&userOutputMutex.semaphore);
        if (strcmp(arg1, "file") == 0) sem_wait(&fileMutex.semaphore);
    }
    else if (strcmp(command, "semSignal") == 0) {
        if (strcmp(arg1, "userInput") == 0) sem_signal(&userInputMutex.semaphore);
        if (strcmp(arg1, "userOutput") == 0) sem_signal(&userOutputMutex.semaphore);
        if (strcmp(arg1, "file") == 0) sem_signal(&fileMutex.semaphore);
    }
    else if (strcmp(command, "assign") == 0) {
        if (strcmp(arg1, "a") == 0) {
            if (strcmp(arg2, "input") == 0) {
                assign("a", file_name);
                memory[process->mem_start + 3].value = atoi(file_name);
            }
            else if (strcmp(arg2, "readFile") == 0) {
                read_file(file_name, buffer, sizeof(buffer));
                memory[process->mem_start + 3].value = atoi(buffer);
            }
        }
        if (strcmp(arg1, "b") == 0) {
            if (strcmp(arg2, "input") == 0) {
                assign("b", file_data);
                memory[process->mem_start + 4].value = atoi(file_data);
            }
            else if (strcmp(arg2, "readFile") == 0) {
                read_file(file_name, buffer, sizeof(buffer));
                memory[process->mem_start + 4].value = atoi(buffer);
            }
        }
    }
    else if (strcmp(command, "writeFile") == 0) {
        write_file(file_name, file_data);
    }
    else if (strcmp(command, "readFile") == 0) {
        read_file(file_name, buffer, sizeof(buffer));
    }
    else if (strcmp(command, "printFromTo") == 0) {
        print_from_to(memory[process->mem_start + 3].value, memory[process->mem_start + 4].value);
    }
    else if (strcmp(command, "print") == 0) {
        print(buffer);
    }
}

void execute_program(Process* process) {
    FILE* file = fopen(process->program_file, "r");
    if (file == NULL) {
        printf("Error opening program file %s\n", process->program_file);
        process->state = DEAD; // Mark process as dead if file can't be opened
        return;
    }

    char instruction[100];
    int time_spent = 0;

    fseek(file, process->program_counter, SEEK_SET);

    while (fgets(instruction, sizeof(instruction), file) && time_spent < time_quantum) {
        execute_instruction(process, instruction);
        time_spent++;
        process->program_counter = ftell(file);
        print_memory();
    }

    if (feof(file)) {
        process->state = DEAD;
    }

    fclose(file);
}

void enqueue(int pid) {
    if ((rear + 1) % QUEUE_SIZE == front) {
        printf("Ready queue is full\n");
        return;
    }
    if (front == -1) front = 0;
    rear = (rear + 1) % QUEUE_SIZE;
    ready_queue[rear] = pid;
}

int dequeue() {
    if (front == -1) {
        printf("Ready queue is empty\n");
        return -1;
    }
    int pid = ready_queue[front];
    if (front == rear) {
        front = rear = -1;
    }
    else {
        front = (front + 1) % QUEUE_SIZE;
    }
    return pid;
}

void add_arriving_processes(Process* processes, int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].state == READY && processes[i].arrival_time == current_time) {
            enqueue(processes[i].pid);
        }
    }
}

void run_scheduler(Process* processes, int num_processes) {
    while (1) {
        add_arriving_processes(processes, num_processes);

        int pid = dequeue();
        if (pid == -1) {
            // Check if all processes are dead
            int all_dead = 1;
            for (int i = 0; i < num_processes; i++) {
                if (processes[i].state != DEAD) {
                    all_dead = 0;
                    break;
                }
            }
            if (all_dead) break;
            current_time++;
            continue;
        }

        Process* process = &processes[pid - 1];

        if (process->state == READY || process->state == RUNNING) {
            process->state = RUNNING;
            printf("\nExecuting process %d\n", process->pid);
            execute_program(process);

            if (process->state != DEAD) {
                process->state = READY;
                enqueue(process->pid);
            }
        }

        current_time++;
    }
}

void allocate_memory(Process* processes, int num_processes) {
    int mem_alloc = 0;
    for (int i = 0; i < num_processes; i++) {
        processes[i].mem_start = mem_alloc;
        snprintf(memory[mem_alloc].name, sizeof(memory[mem_alloc].name), "PID");
        memory[mem_alloc++].value = processes[i].pid;

        snprintf(memory[mem_alloc].name, sizeof(memory[mem_alloc].name), "State");
        memory[mem_alloc++].value = processes[i].state;

        snprintf(memory[mem_alloc].name, sizeof(memory[mem_alloc].name), "PC");
        memory[mem_alloc++].value = processes[i].program_counter;

        snprintf(memory[mem_alloc].name, sizeof(memory[mem_alloc].name), "VarA");
        memory[mem_alloc++].value = 0;

        snprintf(memory[mem_alloc].name, sizeof(memory[mem_alloc].name), "VarB");
        memory[mem_alloc++].value = 0;

        snprintf(memory[mem_alloc].name, sizeof(memory[mem_alloc].name), "VarC");
        memory[mem_alloc++].value = 0;
    }
}

int main() {
    userInputMutex.semaphore = 1;
    userOutputMutex.semaphore = 1;
    fileMutex.semaphore = 1;
    Process processes[NUM_PROCESSES] = {
        { 1, READY, 0, "Program_1.txt", {0}, 0, 0 },
        { 2, READY, 0, "Program_2.txt", {0}, 10, 0 },
        { 3, READY, 0, "Program_3.txt", {0}, 20, 0 }
    };

    printf("Enter the time quantum: ");
    scanf("%d", &time_quantum);

    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("Enter arrival time for process %d: ", i + 1);
        scanf("%d", &processes[i].arrival_time);
    }

    allocate_memory(processes, NUM_PROCESSES);

    run_scheduler(processes, NUM_PROCESSES);

    return 0;
}

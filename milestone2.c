#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MEMORY_SIZE 60
#define PROCESS_VARS_SIZE 3

struct pcb {
    int pid;
    int priority;
    char *state;
    int program_counter;
    int memory_boundaries[2];
};

struct memory_data {
    int memory_words[MEMORY_SIZE];
    int variables[PROCESS_VARS_SIZE];
    struct pcb process_table[3];
};

struct scheduler { 
    struct pcb queue1[3];
    struct pcb queue2[3];
    struct pcb queue3[3];
    struct pcb queue4[3];
    struct pcb blocked_queue[3];
};

void allocate_memory(struct pcb *process, int start_address) {
    process->memory_boundaries[0] = start_address;
    process->memory_boundaries[1] = start_address + PROCESS_VARS_SIZE; 
}

void free_memory(struct pcb *process) {
    process->memory_boundaries[0] = -1;
    process->memory_boundaries[1] = -1;
}

void print(char *str)
{
    printf("%s\n", str);
}

void assign(char *variable, int *variables)
{
    int value;
    printf("Enter value: ");
    scanf("%d", &value);

    variables[variable[0] - 'A'] = value;
}

void write_file(char *filename, char *content)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        print("Error opening file");
        return;
    }
    fprintf(file, "%s", content);
    fclose(file);
}

void read_file(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        print("Error opening file");
        return;
    }
    char c;
    while ((c = fgetc(file)) != EOF)
    {
        printf("%c", c);
    }
    fclose(file);
}

void print_from_to(int from, int to)
{
    for (int i = from; i <= to; i++)
    {
        printf("%d\n", i);
    }
}

void sem_wait(
    int *semaphore,
    int *mutex,
    int *waitCount
)
{
    *waitCount = *waitCount + 1;
    if (*waitCount <= 0)
    {
        *mutex = 0;
        while (*waitCount <= 0)
        {
            // Do nothing
        }
        *mutex = 1;
    }
}

void sem_signal(
    int *semaphore,
    int *mutex,
    int *waitCount
)
{
    *waitCount = *waitCount - 1;
    if (*waitCount <= 0)
    {
        *mutex = 1;
    }
}

void create_process(struct pcb *process, int pid, int priority, char *state, int program_counter)
{
    process->pid = pid;
    process->priority = priority;
    process->state = state;
    process->program_counter = program_counter;
}

void destroy_process(struct pcb *process)
{
    process->pid = -1;
    process->priority = -1;
    strcpy(process->state, "DEAD");
    process->program_counter = -1;
}

void add_to_queue(struct pcb *process, struct pcb *queue)
{
    for (int i = 0; i < 3; i++)
    {
        if (queue[i].pid == -1)
        {
            queue[i] = *process;
            break;
        }
    }
}

void execute_program(const char *filename, struct memory_data *memory, int *semaphore, int *mutex, int *waitCount) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, " ");
        if (strcmp(token, "semWait") == 0) {
            char *resource = strtok(NULL, " ");
            sem_wait(semaphore, mutex, waitCount); // Pass appropriate arguments
        } else if (strcmp(token, "assign") == 0) {
            char *var = strtok(NULL, " ");
            int val = atoi(strtok(NULL, " ")); // Convert value to integer
            assign(var, &val); // Pass variable and value
        } else if (strcmp(token, "writeFile") == 0) {
            char *filename = strtok(NULL, " ");
            char *data = strtok(NULL, " ");
            write_file(filename, data); // Pass filename and data
        } else if (strcmp(token, "readFile") == 0) {
            char *filename = strtok(NULL, " ");
            read_file(filename); // Pass filename
        } else if (strcmp(token, "semSignal") == 0) {
            char *resource = strtok(NULL, " ");
            sem_signal(semaphore, mutex, waitCount); // Pass appropriate arguments
        } else if (strcmp(token, "printFromTo") == 0) {
            int start = atoi(strtok(NULL, " "));
            int end = atoi(strtok(NULL, " "));
            print_from_to(start, end); // Pass start and end values
        } else if (strcmp(token, "print") == 0) {
            char *var = strtok(NULL, " ");
            print(var); // Pass variable to print
        }
    }

    fclose(file);
}

int main() {
    struct memory_data memory;
    struct scheduler os_scheduler;

    int semaphore = 0;
    int mutex = 1;
    int waitCount = 0;

    execute_program("Program_1.txt", &memory, &semaphore, &mutex, &waitCount);
    execute_program("Program_2.txt", &memory, &semaphore, &mutex, &waitCount);
    execute_program("Program_3.txt", &memory, &semaphore, &mutex, &waitCount);

    return 0;
}

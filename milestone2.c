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
    int variables[PROCESS_VARS_SIZE];
};

struct Memory {
    char memory_words[MEMORY_SIZE][100];
    struct Process process_table[NUM_PROCESSES];
};

struct Scheduler {
    struct Process* queues[4][NUM_PROCESSES]; 
    struct Process* blocked_queue[NUM_PROCESSES];
};


void allocate_memory(struct Process* process, int start_address) {
    process->memory_boundaries[0] = start_address;
    process->memory_boundaries[1] = start_address + PROCESS_VARS_SIZE;
}

void free_memory(struct Process* process) {
    process->memory_boundaries[0] = -1;
    process->memory_boundaries[1] = -1;
}

void add_to_queue(struct Process* process, struct Scheduler* scheduler) {
    
    int priority = process->priority;
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (scheduler->queues[priority][i] == NULL) {
            scheduler->queues[priority][i] = process;
            break;
        }
    }
}

void sem_wait(int* semaphore, int* mutex, int* waitCount) {
    *waitCount = *waitCount + 1;
    if (*waitCount <= 0) {
        *mutex = 0;
        while (*waitCount <= 0) {
            // Do nothing
        }
        *mutex = 1;
    }
}

void sem_signal(int* semaphore, int* mutex, int* waitCount) {
    *waitCount = *waitCount - 1;
    if (*waitCount <= 0) {
        *mutex = 1;
    }
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

void read_file(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    char c;
    while ((c = fgetc(file)) != EOF) {
        printf("%c", c);
    }
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

void execute_program(const char* filename, struct Memory* memory, struct Scheduler* scheduler) {

    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error opening file %s\n", filename);
        return;
    }

    char line[100];

    int semaphore = 0, mutex = 1, waitCount = 0; 

    char file_name[100], file_data[100]; 

    int val1, val2; 

    int filename_read = 0;

    while (fgets(line, sizeof(line), file)) {

        char* token = strtok(line, " ");

        if (strcmp(token, "semWait") == 0) {

            char* resource = strtok(NULL, " ");

            sem_wait(&semaphore, &mutex, &waitCount);

        }
        else if (strcmp(token, "assign") == 0) {

             char* var = strtok(NULL, " ");

              if (strcmp(var, "a") == 0)

                 assign(var, &file_name[0]); 

              else if (strcmp(var, "b") == 0)

                 assign(var, &file_data[0]); 

              else {

                  printf("Invalid variable name\n");

                  return;
          }
        }
        else if (strcmp(token, "writeFile") == 0) {

             write_file(file_name, file_data); 

         }
        else if (strcmp(token, "readFile") == 0) {

             if (!filename_read) {

                 assign("filename", &file_name[0]); 

                 filename_read = 1;

                 read_file(file_name);

         } 

             read_file(file_name); 

         }

         else if (strcmp(token, "semSignal") == 0) {

              char* resource = strtok(NULL, " ");

              sem_signal(&semaphore, &mutex, &waitCount);

         }
         else if (strcmp(token, "printFromTo") == 0) {

              val1 = atoi(file_name); 

              val2 = atoi(file_data);

              print_from_to(val1, val2); 
     }

        else if (strcmp(token, "print") == 0) {

            char* var = strtok(NULL, " ");

            print(var);

        }
    }

    fclose(file);
}


int main() {
    struct Memory memory;
    struct Scheduler scheduler;

 
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < NUM_PROCESSES; j++) {
            scheduler.queues[i][j] = NULL;
        }
    }
    for (int i = 0; i < NUM_PROCESSES; i++) {
        scheduler.blocked_queue[i] = NULL;
    }

   
    execute_program("Program_1.txt", &memory, &scheduler);
    execute_program("Program_2.txt", &memory, &scheduler);
    execute_program("Program_3.txt", &memory, &scheduler);

    

    return 0;
}

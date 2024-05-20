# Project Overview

This project involves creating a custom scheduler to manage resource usage between different processes.

The scheduler will :

- read and execute code from provided text files
- manage memory allocation for processes
- implement mutexes for mutual exclusion over critical resources
- schedule processes within the system.

## Programs

Three main programs are provided:

- Program 1: Prints numbers between two given numbers (inclusive).
- Program 2: Writes data to a file. The file is assumed to not exist and should always be created.
- Program 3: Prints the contents of a file.

## Process Control Block (PCB)

A PCB is used to store all information about a process.
Each process will have a PCB containing:

- its ID
- state
- current priority
- program counter
- memory boundaries

## Program Syntax

The programs use a specific syntax for various operations such as:

- print
- assign
- writeFile
- readFile
- printFromTo
- semWait
- semSignal
- Each line of instruction in the program will be executed in 1 clock cycle.

## Memory

The OS is responsible for managing memory and its allocation.
The memory is of a fixed size, made up of 60 memory words.
Each process needs enough space for 3 variables.

## scheduler

The scheduler is responsible for scheduling between the processes in the Ready Queue.
A multilevel feedback model scheduling algorithm is to be implemented.

## Mutual Exclusion

Mutexes are used to ensure mutual exclusion over the critical section.
Three mutexes are to be implemented for accessing a file, taking user input, and outputting on the screen.

## Queues

There are 4 Ready Queues for the processes currently waiting to be chosen to execute on the processor and a General Blocked Queue for the processes currently waiting for any resource to be available.

## Deliverables

The Simulated OS should be able to read the provided programs and execute them.

### Outputs should include

- the state of queues after every scheduling event
- the currently executing process and instruction
- the order in which processes are scheduled
- the timings in which processes arrive
- The memory should be shown every clock cycle in a human-readable format

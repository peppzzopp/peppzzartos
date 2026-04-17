# PEPPZZARTOS

A minimal RTOS built from scratch to explore scheduling, synchronization, and low-level system design.

---

## Core Features

### Fair Round-Robin Scheduling
Tasks are scheduled in a cyclic order, ensuring equal CPU time distribution without starvation.

### Idle (Kernel Delay) Task
A dedicated low-priority task runs when no other tasks are ready, ensuring the CPU is never left unmanaged.

### Synchronization Primitives
- **Mutexes** for mutual exclusion  
- **Semaphores** for signaling and resource counting  

### Task Lifecycle Management
- Tasks can **self-delete safely**
- Deleting a task that holds a mutex or a semaphore is **undefined and unsafe**

### Optional timing analysis
- Optional build configuration to enable timing analysis.
- Allows measurement of
    - Context switch latency
    - Scheduler tick latency
- Designed to be inspected using **GDB**.

---

## Notes/ Limitations
- No concept of priority. All tasks are considered equal in priority.
- This scheduler is designed for learning, experimentation and low-level system understanding.
- Edge cases outside tested scenarios may exhibit undefined behavior.

---

## Demo application
- A UART-based shell is provided to demonstrate kernel functionality.
- The shell supports a minimal set of commands to interact with tasks and synchronization primitives.
- To access, build the project on supported hardware and then use **screen** or **minicom** to interact with the shell.
- Type help to see the available commands.

---

## Building the project
- To build the project, clone this repository and run ```make```.
- Currently the project can be run only on the **STM32F103RB** Nucleo board.
- Information regarding porting this scheduler to any other architectures is given in [porting](docs/port.md).

# Architecture

## Overview

PEPPZZARTOS is a minimal bare-metal RTOS built around a preemptive round-robin scheduler.  
The system consists of:

- Tasks represented by lightweight control blocks
- A tick-driven scheduler
- Synchronization primitives (mutexes and semaphores)
- A deferred context switching mechanism

---

## Task Model

Tasks are represented using a lightweight structure:

```c
typedef struct{
    uint32_t *stack_pointer;
    volatile uint32_t delay_timer;
    void (*task_function)(void);
} task_t;
```

Each task contains:
- A stack pointer to its execution context
- A delay timer used for timed blocking
- A function pointer representing the task entry

### Memory Layout

- Each task is allocated a fixed-size stack (40 words, can be changed in kernel.c)
- The task control block resides within the same allocation
- Task allocation is managed using a bitmask-based allocator

---

## Scheduler Design

The scheduler follows a **round-robin policy** with emphasis on simplicity and fairness.

- Tasks are tracked using:
  - A **ready bitmask**
  - A **delayed bitmask**

- On every system tick:
  - Delayed tasks are updated
  - The next runnable task is selected

- If the selected task differs from the current task:
  - A context switch is requested via `kernel_yield()`

### Fairness Mechanism

- The scheduler tracks the last executed task
- After idle periods, scheduling resumes from the last active task
- This avoids bias toward lower-indexed tasks

### Idle Task

- If no task is ready, the scheduler executes the idle (sleep) task

---

## Synchronization

PEPPZZARTOS provides:
- **Mutexes** (mutual exclusion)
- **Semaphores** (resource counting)

### Blocking Behavior

- If a mutex is already owned:
  - The requesting task is removed from the ready bitmask
  - It is placed into a mutex wait queue

- If a semaphore is unavailable:
  - The task is removed from the ready bitmask
  - It is placed into a semaphore wait queue
  - The task retries acquisition when rescheduled

### Wake-up Policy

- **Mutex:**
  - Ownership is handed over in a round-robin fashion among waiting tasks
  - Only one task proceeds at a time

- **Semaphore:**
  - A single waiting task is selected and marked ready
  - The task re-attempts acquisition upon execution

---

## Delay and Task Deletion

### Delay Mechanism

- Tasks can be delayed using a tick-based timer
- Delayed tasks are tracked via a bitmask
- On each tick:
  - `delay_timer` is decremented
  - Tasks reaching zero are moved back to the ready set

---

### Task Deletion

- Tasks can delete themselves or be deleted externally
- On deletion:
  - The task is removed from both ready and delayed bitmasks
  - Stack cleanup is performed during the tick handler

### Safety Notes

- Tasks waiting on mutexes or semaphores can be safely deleted
- Deleting a task that holds a mutex or has acquired a semaphore is **unsafe and results in undefined behavior**

---

## Context Switching (Overview)

- Context switching is deferred and handled via a dedicated interrupt
- Triggered using `kernel_yield()`
- The context switch handler:
  - Saves the current task context
  - Restores the next task context

Refer to the [Porting Guide](port.md) for architecture-specific implementation details.

---

## Interrupt Flow

The system operates based on the following flow:

```
Task -> Tick Interrupt -> kernel_ticks() -> Scheduler Decision -> kernel_yield() -> Context Switch Interrupt -> Task Switch
```

---

## Design Decisions & Tradeoffs

- **No priority scheduling**
  - Simplifies implementation and avoids priority inversion handling

- **Bitmask-based task tracking**
  - Enables fast scheduling decisions with minimal overhead

- **Fixed stack size per task**
  - Reduces allocation complexity at the cost of flexibility

- **No resource ownership tracking for semaphores and mutexes**
  - Keeps implementation simple but makes certain deletions unsafe

- **Deferred context switching**
  - Keeps interrupt handlers lightweight and predictable

---
## Hardware Abstraction

The current implementation includes minimal drivers for:
- UART
- GPIO
- SysTick (enabling them for the microcontroller)

These drivers are used to support the kernel and demo application but are not tightly coupled to the scheduler design.

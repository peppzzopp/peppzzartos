# Porting Guide

Porting PEPPZZARTOS to a new architecture requires implementing a small set of architecture-specific components defined by the kernel–port contract.

---

## Kernel–Port Contract

The interface between the portable kernel and the architecture-specific code is defined in `kernel_internal.h`.

The following functions must be implemented or provided:

- **kernel_ticks()**
  - Invoked on every system tick
  - Defined in `kernel.c`
  - Responsible for advancing the scheduler state

- **kernel_yield()**
  - Triggers a context switch by pending the context switch interrupt
  - Must be implemented as part of the port

- **kernel_enter_critical()**
  - Disables interrupts to enter a critical section
  - Must be implemented as part of the port

- **kernel_exit_critical()**
  - Re-enables interrupts after a critical section
  - Must be implemented as part of the port

  **kernel_timing_init()**
  - Optional provision for debugging timing related issues.
  - Mentioned in detail [here](timing.md).

---

## Interrupts and Service Routines

Preemptive scheduling requires periodic interruption of the currently running task.

Two interrupts are required for correct operation:

- **Tick Interrupt**
  - Fires at a fixed interval
  - Drives the scheduler
  - ISR must call:
    ```c
    kernel_ticks();
    ```

- **Context Switch Interrupt**
  - A pendable interrupt used to perform context switching
  - Triggered via `kernel_yield()`
  - ISR is responsible for saving and restoring task context

### Example (STM32F103)

- **SysTick** → Tick interrupt  
- **PendSV** → Context switch interrupt  

### Required Variables

The context switch interrupt handler must use the **current_task** and **next_task** variables and switch the context based on those variables.

---

## Reference Implementation

An example implementation for the STM32F103 platform is available in the `port/` directory and can be used as a reference while porting to other architectures.

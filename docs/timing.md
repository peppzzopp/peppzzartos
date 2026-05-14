# Timing & Instrumentation

PEPPZZARTOS provides optional support for low-level timing analysis using **GDB** and **OpenOCD**.  
This allows measurement of scheduler-related latencies such as context switch time and tick handling duration.

---

## Enabling Timing Support

Timing instrumentation is enabled at compile time using:

```bash
-DPEPPZZARTOS_TIMING_ENABLE
```

If using the provided Makefile:

```bash
make timing (DEVICE=riscv)
```

---

## Implementation

### Cycle Counter Initialization

For each architecture, the function `kernel_timing_init()` must be implemented.

- This function should:
  - Enable and initialize a hardware cycle counter
  - Ensure the counter increments at CPU frequency (or a known rate)
**Note**: For specific architectures where cycle count has to be enabled from reset like riscv, the initialization function can be empty.

---

### Instrumentation Points

Timing measurements must be inserted at the following points:

- **Tick Interrupt**
- **Context Switch Interrupt**

Refer to the [Porting Guide](port.md) for details on these interrupts.

---

### Required Variables

#### STM32 port
The following global variables must be defined for compatibility with the provided GDB script:

- `tick_start`
- `tick_end`
- `context_switch_start`
- `context_switch_end`

These variables should store cycle counter values at appropriate points in execution.

#### RISCV port
RiscV port of the kernel follows minimal trap handling where execution jumps to common trap handler for any trap.
- `trap_start`
- `trap_end`
These variables must be present and store appropriate values to be compatible with the timing measurement system provided by [peppzzemcu](https://github.com/peppzzopp/peppzzemcu)

---

### Conditional Compilation

All timing-related code must be enclosed within:

```c
#ifdef PEPPZZARTOS_TIMING_ENABLE
// timing code
#endif
```

This ensures zero overhead when timing is disabled.

---

## Usage
### STM32 port
1. Connect to the target using **OpenOCD** and **GDB**
2. Load the firmware with timing enabled
3. From within GDB, source the provided script:

```gdb
source timing_measure.gdb
```

4. The script will:
   - Print timing statistics to the console
   - Log results to `timing_results.txt`

### RISCV port
1. This port is intended to be run on [peppzzemcu](https://github.com/peppzzopp/peppzzemcu) project.
2. So clone the project repo and run the commads
```bash
git clone https://github.com/peppzzopp/peppzzemcu --recursive
cd peppzzemcu
make perf
```
3. Results will be stored in performance_log.txt.
---

## Measured Results

### STM32 port
Measurements were obtained using the instrumentation described above on the **STM32F103RB** platform for the shell demo.

- **CPU Frequency**: 8 MHz
- **Tick Frequency**: 1 kHz (1 ms period)

- **Tick Handler Duration**: ~443 cycle 
- **Context Switch Latency**: 74 cycle 

This corresponds to approximately:
- Tick handler: ~55 microsec
- Context switch: ~9 microsec

### RISCV port
Measurements were obtained using the instrumentation described above on the **[peppzzemcu](https://github.com/peppzzopp/peppzzemcu)** platform for the fpga demo.

- **CPU Frequency**: 27 MHz
- **Tick Frequency**: 1 kHz (1 ms period)

- **Maximum Trap Overhead**: 1976 cycle 
- **Minimum Trap Overhead**: 987 cycle 
- **Average Trap Overhead**: 1013 cycle 

This corresponds to approximately:
- Maximum : 73.19 microsec
- Minimum : 36.56 microsec
- Average : 37.52 microsec

### Notes on Measurement

- Measurements are taken using the hardware cycle counter
- Values may vary depending on compiler optimizations and debug configuration
- These results are intended for relative performance analysis

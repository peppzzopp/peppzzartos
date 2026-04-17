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
make timing
```

---

## Implementation

### Cycle Counter Initialization

For each architecture, the function `kernel_timing_init()` must be implemented.

- This function should:
  - Enable and initialize a hardware cycle counter
  - Ensure the counter increments at CPU frequency (or a known rate)

---

### Instrumentation Points

Timing measurements must be inserted at the following points:

- **Tick Interrupt**
- **Context Switch Interrupt**

Refer to the [Porting Guide](port.md) for details on these interrupts.

---

### Required Variables

The following global variables must be defined for compatibility with the provided GDB script:

- `tick_start`
- `tick_end`
- `context_switch_start`
- `context_switch_end`

These variables should store cycle counter values at appropriate points in execution.

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

1. Connect to the target using **OpenOCD** and **GDB**
2. Load the firmware with timing enabled
3. From within GDB, source the provided script:

```gdb
source timing_measure.gdb
```

4. The script will:
   - Print timing statistics to the console
   - Log results to `timing_results.txt`

---

## Measured Results

Measurements were obtained using the instrumentation described above on the **STM32F103RB** platform for the shell demo.

- **CPU Frequency**: 8 MHz  
- **Tick Frequency**: 1 kHz (1 ms period)

- **Tick Handler Duration**: ~443 cycles  
- **Context Switch Latency**: 74 cycles  

This corresponds to approximately:
- Tick handler: ~55 µs  
- Context switch: ~9 µs  

### Notes on Measurement

- Measurements are taken using the hardware cycle counter
- Values may vary depending on compiler optimizations and debug configuration
- These results are intended for relative performance analysis

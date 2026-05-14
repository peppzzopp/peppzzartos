PROJECT      := peppzzartos
BUILD        := build
BUILD_NORMAL := $(BUILD)/normal
BUILD_TIMING := $(BUILD)/timing

# Default device if none is specified
DEVICE ?= stm32f1

# ---------------------------------------------------------
# Device-Specific Configurations
# ---------------------------------------------------------
ifeq ($(DEVICE),stm32f1)
    CROSS_COMPILE := arm-none-eabi-
    CFLAGS        := -mcpu=cortex-m3 -mthumb -g
    LDSCRIPT      := port/stm32f1/stm32_linker.ld
    PORT_INC      := -I port/stm32f1/inc
    PORT_SRC      := app/uart_shell.c \
					 port/stm32f1/src/kernel_internal.c \
                     port/stm32f1/src/startup.c \
                     port/stm32f1/src/usart.c \
                     port/stm32f1/src/gpio.c \
                     port/stm32f1/src/systick.c
    
    # OpenOCD specific config for STM32
    OOCD_IFACE    := stlink.cfg
    OOCD_TARGET   := stm32f1x.cfg

else ifeq ($(DEVICE),riscv)
    # Adjust prefix if your toolchain is different (e.g., riscv64-unknown-elf-)
    CROSS_COMPILE := riscv-none-elf-
    # Using your specific architecture string from our previous chats
    CFLAGS        := -march=rv32i_zicsr -mabi=ilp32 -g
    
    # Update these paths to match wherever you put your RISC-V port
    LDSCRIPT      := port/riscv/riscv_linker.ld
    PORT_INC      := -I port/riscv/inc
    PORT_SRC      := app/fpga.c \
					 port/riscv/src/kernel_internal.c \
                     port/riscv/src/startup.c \
					 port/riscv/src/timer.c
    
    # OpenOCD config for your RISC-V board (update as needed)
    OOCD_IFACE    := ftdi/dp_busblaster.cfg # Example
    OOCD_TARGET   := riscv_jtag.cfg         # Example

else
    $(error Unsupported DEVICE: $(DEVICE). Please use DEVICE=stm32f1 or DEVICE=riscv)
endif

# ---------------------------------------------------------
# Toolchain & Global Flags
# ---------------------------------------------------------
CC      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy

LDFLAGS_NORMAL := -nostdlib -T $(LDSCRIPT) -Wl,-Map=$(BUILD_NORMAL)/$(PROJECT).map
LDFLAGS_TIMING := -nostdlib -T $(LDSCRIPT) -Wl,-Map=$(BUILD_TIMING)/$(PROJECT).map

TIMING_FLAG := -DPEPPZZARTOS_TIMING_ENABLE
OPENOCD     := /usr/share/openocd/scripts

# Merge Common and Port-Specific Includes/Sources
INC_DIRS := -I kernel/inc $(PORT_INC)

SRC_FILES := kernel/src/kernel.c \
             kernel/src/sync/mutex.c \
             kernel/src/sync/semaphore.c \
             $(PORT_SRC)

# ---------------------------------------------------------
# Build Targets
# ---------------------------------------------------------
OBJS      := $(SRC_FILES:%.c=$(BUILD_NORMAL)/%.o)
OBJS_TIME := $(SRC_FILES:%.c=$(BUILD_TIMING)/%.o)

ELF      := $(BUILD_NORMAL)/$(PROJECT).elf
BIN      := $(BUILD_NORMAL)/$(PROJECT).bin
ELF_TIME := $(BUILD_TIMING)/$(PROJECT).elf
BIN_TIME := $(BUILD_TIMING)/$(PROJECT).bin

.PHONY: all timing load clean

all: $(ELF) $(BIN)

timing: $(ELF_TIME) $(BIN_TIME)

$(BUILD_NORMAL):
	mkdir -p $(BUILD_NORMAL)

$(BUILD_TIMING):
	mkdir -p $(BUILD_TIMING)

$(BUILD_NORMAL)/%.o: %.c | $(BUILD_NORMAL)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_DIRS) -c $< -o $@

$(BUILD_TIMING)/%.o: %.c | $(BUILD_TIMING)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TIMING_FLAG) $(INC_DIRS) -c $< -o $@

$(ELF): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS_NORMAL) -o $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(ELF_TIME): $(OBJS_TIME)
	$(CC) $(OBJS_TIME) $(LDFLAGS_TIMING) -o $@

$(BIN_TIME): $(ELF_TIME)
	$(OBJCOPY) -O binary $< $@

load:
	@if [ -f $(ELF_TIME) ]; then \
		openocd -f $(OPENOCD)/interface/$(OOCD_IFACE) \
		        -f $(OPENOCD)/target/$(OOCD_TARGET) \
		        -c "program $(ELF_TIME) verify reset exit"; \
	elif [ -f $(ELF) ]; then \
		openocd -f $(OPENOCD)/interface/$(OOCD_IFACE) \
		        -f $(OPENOCD)/target/$(OOCD_TARGET) \
		        -c "program $(ELF) verify reset exit"; \
	else \
		echo "No binary found, run make or make timing first"; \
	fi

clean:
	rm -rf $(BUILD)

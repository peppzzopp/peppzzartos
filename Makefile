PROJECT      := peppzzartos
BUILD        := build
BUILD_NORMAL := $(BUILD)/normal
BUILD_TIMING := $(BUILD)/timing

CC   := arm-none-eabi
CPU  := cortex-m3
CFLAGS := -mcpu=$(CPU) -mthumb -g

LDFLAGS_NORMAL := -nostdlib -T port/stm32f1/stm32_linker.ld -Wl,-Map=$(BUILD_NORMAL)/$(PROJECT).map
LDFLAGS_TIMING := -nostdlib -T port/stm32f1/stm32_linker.ld -Wl,-Map=$(BUILD_TIMING)/$(PROJECT).map

TIMING_FLAG := -DPEPPZZARTOS_TIMING_ENABLE
OPENOCD     := /usr/share/openocd/scripts

INC_DIRS := -I kernel/inc \
            -I port/stm32f1/inc

SRC_FILES := app/main.c \
             kernel/src/kernel.c \
             kernel/src/sync/mutex.c \
			 kernel/src/sync/semaphore.c \
             port/stm32f1/src/kernel_internal.c \
             port/stm32f1/src/startup.c \
             port/stm32f1/src/usart.c \
             port/stm32f1/src/gpio.c \
             port/stm32f1/src/systick.c

OBJS      := $(SRC_FILES:%.c=$(BUILD_NORMAL)/%.o)
OBJS_TIME := $(SRC_FILES:%.c=$(BUILD_TIMING)/%.o)

ELF      := $(BUILD_NORMAL)/$(PROJECT).elf
BIN      := $(BUILD_NORMAL)/$(PROJECT).bin
ELF_TIME := $(BUILD_TIMING)/$(PROJECT).elf
BIN_TIME := $(BUILD_TIMING)/$(PROJECT).bin

all: $(ELF) $(BIN)

timing: $(ELF_TIME) $(BIN_TIME)

$(BUILD_NORMAL):
	mkdir -p $(BUILD_NORMAL)

$(BUILD_TIMING):
	mkdir -p $(BUILD_TIMING)

$(BUILD_NORMAL)/%.o: %.c | $(BUILD_NORMAL)
	mkdir -p $(dir $@)
	$(CC)-gcc $(CFLAGS) $(INC_DIRS) -c $< -o $@

$(BUILD_TIMING)/%.o: %.c | $(BUILD_TIMING)
	mkdir -p $(dir $@)
	$(CC)-gcc $(CFLAGS) $(TIMING_FLAG) $(INC_DIRS) -c $< -o $@

$(ELF): $(OBJS)
	$(CC)-gcc $(OBJS) $(LDFLAGS_NORMAL) -o $@

$(BIN): $(ELF)
	$(CC)-objcopy -O binary $< $@

$(ELF_TIME): $(OBJS_TIME)
	$(CC)-gcc $(OBJS_TIME) $(LDFLAGS_TIMING) -o $@

$(BIN_TIME): $(ELF_TIME)
	$(CC)-objcopy -O binary $< $@

load:
	@if [ -f $(ELF_TIME) ]; then \
		openocd -f $(OPENOCD)/interface/stlink.cfg \
		        -f $(OPENOCD)/target/stm32f1x.cfg \
		        -c "program $(ELF_TIME) verify reset exit"; \
	elif [ -f $(ELF) ]; then \
		openocd -f $(OPENOCD)/interface/stlink.cfg \
		        -f $(OPENOCD)/target/stm32f1x.cfg \
		        -c "program $(ELF) verify reset exit"; \
	else \
		echo "No binary found, run make or make timing first"; \
	fi

clean:
	rm -rf $(BUILD)

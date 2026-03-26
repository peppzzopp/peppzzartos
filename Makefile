PROJECT := peppzzartos
BUILD := build

CC=arm-none-eabi
CPU=cortex-m3
CFLAGS= -c -mcpu=$(CPU) -mthumb -g -o0
LDFLAGS= -nostdlib -T stm32_linker.ld -Wl,-Map=$(BUILD)/$(PROJECT).map

OPENOCD=/usr/share/openocd/scripts

INC_DIRS := -I kernel/inc \
		  -I port/stm32f1/inc
SRC_FILES := app/main.c \
			 kernel/src/kernel.c \
			 port/stm32f1/src/kernel_internal.c \
			 port/stm32f1/src/startup.c \
			 port/stm32f1/src/usart.c \
			 port/stm32f1/src/gpio.c \
			 port/stm32f1/src/systick.c

OBJS := $(SRC_FILES:%.c=$(BUILD)/%.o)

ELF := $(BUILD)/$(PROJECT).elf
BIN := $(BUILD)/$(PROJECT).bin

all: $(ELF) $(BIN)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: %.c | $(BUILD)
	mkdir -p $(dir $@)
	$(CC)-gcc $(CFLAGS) $(INC_DIRS) -c $< -o $@

$(ELF): $(OBJS)
	$(CC)-gcc $(OBJS) $(LDFLAGS) -o $@

$(BIN): $(ELF)
	$(CC)-objcopy -O binary $< $@

load: $(ELF) 
	openocd -f $(OPENOCD)/interface/stlink.cfg -f $(OPENOCD)/target/stm32f1x.cfg -c "program $^ verify reset exit"

clean: $(BUILD)
	rm -rf $(BUILD)

CC=arm-none-eabi-gcc
CPU=cortex-m3
CFLAGS= -c -mcpu=$(CPU) -mthumb -g -o0
LDFLAGS= -nostdlib -T stm32_linker.ld -Wl,-Map=final.map

OPENOCD=/usr/share/openocd/scripts

all: main.o stm32_startup.o usart.o scheduler.o final.elf

main.o: main.c
	$(CC) $(CFLAGS) $^ -o $@
usart.o: usart.c 
	$(CC) $(CFLAGS) $^ -o $@
scheduler.o: scheduler.c 
	$(CC) $(CFLAGS) $^ -o $@
stm32_startup.o: stm32_startup.c
	$(CC) $(CFLAGS) $^ -o $@
final.elf: main.o stm32_startup.o usart.o scheduler.o
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -rf *.o *.elf *.out *.S *.map

load: final.elf
	openocd -f $(OPENOCD)/interface/stlink.cfg -f $(OPENOCD)/target/stm32f1x.cfg -c "program $^ verify reset exit"

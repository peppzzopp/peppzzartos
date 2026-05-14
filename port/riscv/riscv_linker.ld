OUTPUT_ARCH("riscv")
ENTRY(_stack_setup)

MEMORY
{
    ROM (rx)  : ORIGIN = 0x80000000, LENGTH = 16K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}

SECTIONS
{
    .text :
    {
        *(.text.entry)
        *(.text)
        *(.rodata)
        . = ALIGN(4);
        _etext = .;
    }> ROM 

    .data :
    {
        _sdata = .;
        *(.sdata)
        *(.data)
        . = ALIGN(4);
        _edata = .;
    }> RAM AT> ROM

    .bss :
    {
        _sbss = .;
        *(.sbss)
        *(.bss)
        . = ALIGN(4);
        _ebss = .;
        heap_start = .;
    }> RAM

    _stack = ORIGIN(RAM) + LENGTH(RAM);
}

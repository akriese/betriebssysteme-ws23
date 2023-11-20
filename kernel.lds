ENTRY(_start)

MEMORY
{
    /* Define memory regions */
    SRAM (xrw) : ORIGIN = 0x00200000, LENGTH = 16K
    RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 64M
}

SECTIONS
{
.init : { *(.init) } > SRAM
.text : { *(.text) } > RAM
}

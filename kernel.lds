ENTRY(_start)
SECTIONS
{
    . = 0x20000000;
    init : {
        system/start_asm.o(.text)
    }

    .text : {
        *(.text)
    }
}

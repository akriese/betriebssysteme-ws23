ENTRY(_start)
SECTIONS
{
    . = 0x20000000;
    init : {
        system/start.o(.text)
    }

    .text : {
        *(.text)
    }
}

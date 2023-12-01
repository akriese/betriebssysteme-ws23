target remote localhost:1234

set architecture arm
file kernel
tui enable
layout asm
layout regs

break setup_kernel

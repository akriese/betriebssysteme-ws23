target remote localhost:1234

set architecture arm
file kernel
tui enable
layout asm
layout regs

alias N = nexti

break setup_kernel
break _Lend
break scheduler_next_asm
break handle_sys_interrupt
break dbgu_receive_interrupt_handler
break idling
# break .somelabel
# break print
break dbgu_create_thread_on_interrupt

break lib/programs/threads.c:27

break *0x0
break *0x4
break *0x8
break *0xc
break *0x10
break *0x14
# break *0x18
break *0x1c

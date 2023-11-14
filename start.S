.section .init
.global _start

_start:
        @ simply execute the main OS function
        b main

@ infinite loop
.Lend:
        b .Lend

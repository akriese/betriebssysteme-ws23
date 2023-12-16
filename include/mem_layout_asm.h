// some definitions that are needed by assembler source files

#ifndef _MEM_LAYOUT_ASM_H
#define _MEM_LAYOUT_ASM_H

#define KiB (1024 * 1)
#define MiB (1024 * KB)
#define GiB (1024 * MB)

/* Stacks tummeln sich am Ende des RAMs */
#define STACK_BOTTOM 0x24000000

/* 64k should be enough for everyone */
#define STACK_SIZE (64 * KiB)

#endif /* ifndef _MEM_LAYOUT_ASM_H */

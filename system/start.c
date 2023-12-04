#include "memlayout.h"
#include <mem.h>
#include <mem_addresses.h>

enum cpu_mode {
  CPU_MODE_USER = 0x10,
  CPU_MODE_FIQ = 0x11,
  CPU_MODE_IRQ = 0x12,
  CPU_MODE_SVC = 0x13,
  CPU_MODE_ABT = 0x17,
  CPU_MODE_UND = 0x1b,
  CPU_MODE_SYS = 0x1f,
};

// defined in system/exceptions_asm.S
extern char _ivt_start[];
extern char _ivt_end[];

extern void _set_cpu_mode_stack(enum cpu_mode mode, unsigned int stack_size);

void setup_kernel(void) {
  // init stacks for different cpmodes
  _set_cpu_mode_stack(CPU_MODE_FIQ, STACK_BOTTOM - 1 * STACK_SIZE);
  _set_cpu_mode_stack(CPU_MODE_IRQ, STACK_BOTTOM - 2 * STACK_SIZE);
  _set_cpu_mode_stack(CPU_MODE_UND, STACK_BOTTOM - 3 * STACK_SIZE);
  _set_cpu_mode_stack(CPU_MODE_ABT, STACK_BOTTOM - 4 * STACK_SIZE);
  _set_cpu_mode_stack(CPU_MODE_SYS, STACK_BOTTOM - 5 * STACK_SIZE);

  // init exceptions
  memcpy(_ivt_start, (void *)INTERNAL_RAM, _ivt_end - _ivt_start);
  mc_remap();
}

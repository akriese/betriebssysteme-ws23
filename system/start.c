#include <mem.h>
#include <mem_addresses.h>
#include <mem_layout.h>
#include <system.h>

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

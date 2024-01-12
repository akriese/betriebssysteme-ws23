#include <mem.h>
#include <mem_addresses.h>
#include <mem_layout.h>
#include <system.h>

#include <dbgu.h>
#include <scheduler.h>

// defined in system/exceptions_asm.S
extern char _ivt_start[];
extern char _ivt_end[];

extern void _set_cpu_mode_stack(cpu_mode mode, unsigned int stack_size);

int idle_fun(void *__unused) {
  // simply doing an infinite loop will lead to an assembly of one instruction
  // which leads to weird behaviour when the thread is started again.
  // That is why, we insert this dummy variable here
  volatile int j = 0;
  while (1) {
    j++;
  }

  return 0;
}

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

  // initialize the dbgu and enable its interrupts
  dbgu_initialize();

  // install handler for system interrupts (dbgu and system timer, currently)
  init_sys_interrupts();

  // enable system timer interrupts and set time
  st_activate_pits(100);

  // initialize the scheduler
  scheduler_init(idle_fun);

  // enable interrupts
  cpsr_enable_interrupts();
}

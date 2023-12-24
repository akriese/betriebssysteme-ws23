#ifndef _SYSTEM_H_
#define _SYSTEM_H_

enum cpu_mode {
  CPU_MODE_USER = 0x10,
  CPU_MODE_FIQ = 0x11,
  CPU_MODE_IRQ = 0x12,
  CPU_MODE_SVC = 0x13,
  CPU_MODE_ABT = 0x17,
  CPU_MODE_UND = 0x1b,
  CPU_MODE_SYS = 0x1f,
};

enum interrupt_handler_routines {
  SYSTEM_TIMER_HANDLER,
  DBGU_RECEIVE_HANDLER,
  _INTERRUPT_HANDLER_ROUTINES_END // used to find out the number of routines
};

void mc_remap();

void st_activate_pits(unsigned short ms);

int st_interrupt_active();

void init_sys_interrupts();

void register_interrupt_routines(enum interrupt_handler_routines routine,
                                 void (*handler)(void *));

void cpsr_enable_interrupts();

#endif /* ifndef _SYSTEM_H_ */

#include <dbgu.h>
#include <mem.h>
#include <mem_addresses.h>
#include <print.h>
#include <scheduler.h>
#include <sys_call.h>
#include <system.h>
#include <thread.h>

#define SYS_INTERRUPT 1

extern void handle_sys_interrupt();

struct aic {
  unsigned int smr[32];
  void *svr[32];
  unsigned int __ivr;
  unsigned int __fvr;
  unsigned int __isr;
  unsigned int __ipr;
  unsigned int __imr;
  unsigned int __cisr;
  unsigned int __reserved[2];
  unsigned int iecr;
  unsigned int idcr;
  unsigned int iccr;
  unsigned int iscr;
  unsigned int eoicr;
  unsigned int spu;
  unsigned int dcr;
};

volatile struct aic *const aic = (struct aic *)AIC;

// array of function pointers holding interrupt routines
void (*handlers[_INTERRUPT_HANDLER_ROUTINES_END])(void *context);

void register_interrupt_routines(enum interrupt_handler_routines routine,
                                 void (*handler)(void *)) {
  handlers[routine] = handler;
}

void system_interrupt_handler(void *context) {
  // read status registers of the system peripherals
  // to determine where the interrupt comes from
  if (st_interrupt_active()) {
    scheduler_count_time();
    scheduler_next(context);
    if (handlers[SYSTEM_TIMER_HANDLER]) {
      handlers[SYSTEM_TIMER_HANDLER](context);
    }
  } else if (dbgu_interrupt_active()) {
    dbgu_receive_interrupt_handler();
    if (handlers[DBGU_RECEIVE_HANDLER]) {
      handlers[DBGU_RECEIVE_HANDLER](context);
    }

    unsigned int unblocked_thread_id = thread_unblock(RESOURCE_DBGU_RECEIVE);
    sys_call_post_unblock(RESOURCE_DBGU_RECEIVE, unblocked_thread_id);
    scheduler_switch(unblocked_thread_id, context);
  }

  // finally, signal that the interrupt handling is over
  aic->eoicr = 1;
}

void init_sys_interrupts() {
  aic->iecr = 1 << SYS_INTERRUPT;
  aic->smr[SYS_INTERRUPT] = 7; // set highest priority
  aic->svr[SYS_INTERRUPT] = *handle_sys_interrupt;

  // reset all custom handlers to 0
  memset(handlers, 0, 4 * _INTERRUPT_HANDLER_ROUTINES_END);

  print("Handlers' address: %p\n\r", handlers);
}

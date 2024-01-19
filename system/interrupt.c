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

typedef struct advanced_interrupt_controller {
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
} advanced_interrupt_controller;

volatile advanced_interrupt_controller *const aic =
    (advanced_interrupt_controller *)AIC;

// array of function pointers holding interrupt routines
void (*handlers[_INTERRUPT_HANDLER_ROUTINES_END])(thread_context *context);

/**
 * @brief Registers a routine to handle a certain interrupt.
 *
 * The routine will receive the current thread's context as argument.
 *
 * @param routine IRQ routine type
 * @param handler Function pointer to the handler.
 */
void register_interrupt_routines(interrupt_handler_routines routine,
                                 void (*handler)(thread_context *)) {
  handlers[routine] = handler;
}

/**
 * @brief Handling interrupts on IRQ line 1 (system).
 *
 * Needs to find out which instance caused the interrupt and then calls
 * routines accordingly.
 *
 * @param context The context of the currently running thread.
 */
void system_interrupt_handler(thread_context *context) {
  // read status registers of the system peripherals
  // to determine where the interrupt comes from
  if (st_interrupt_active()) {
    // count up the system time and wake up threads that were sleeping
    scheduler_count_time();

    // switch to another thread if possible
    scheduler_next(context);

    // execute a handler if registered
    if (handlers[SYSTEM_TIMER_HANDLER]) {
      handlers[SYSTEM_TIMER_HANDLER](context);
    }
  } else if (dbgu_interrupt_active()) {
    // process the received character
    dbgu_receive_interrupt_handler();

    // execute a handler if registered
    if (handlers[DBGU_RECEIVE_HANDLER]) {
      handlers[DBGU_RECEIVE_HANDLER](context);
    }

    // unblock a thread that was waiting for input
    int unblocked_thread_id = thread_unblock(RESOURCE_DBGU_RECEIVE);

    if (unblocked_thread_id != -1) {
      // postprocess the unblocking
      // (in this case, write the input to the thread's context)
      sys_call_post_unblock(RESOURCE_DBGU_RECEIVE, unblocked_thread_id);

      // switch to the unblocked thread immediately
      scheduler_switch(unblocked_thread_id, context);
    }
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
}

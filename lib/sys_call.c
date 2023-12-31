#include <print.h>
#include <scheduler.h>
#include <sys_call.h>
#include <thread.h>

void sys_call_handler(unsigned int number, void *context) {
  switch (number) {
  case 1:
    thread_finish();
    scheduler_next(context);
    break;
  default:
    print("There is currently no handler defined for SWI #%d\n\r", number);
  }
}

void sys_call_exit_thread() { asm("swi #1"); }

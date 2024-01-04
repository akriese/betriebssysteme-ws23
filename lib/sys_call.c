#include <dbgu.h>
#include <print.h>
#include <scheduler.h>
#include <sys_call.h>
#include <system.h>
#include <thread.h>

// some macro magic with variable arguments
// inspired by https://stackoverflow.com/a/11763277
//
// This will only insert the SWI number if nothing else is given.
// If input and output are given, other macros are used.
// This only allows for one input and one ouput each, as otherwise the macro
// has to be more complex
#define __SWI(NUM) __asm__ volatile("swi %0" : : "I"(NUM))
#define __SWI_IN(NUM, INPUT)                                                   \
  __asm__ volatile("mov r0, %1\n\tswi %0" : : "I"(NUM), INPUT)
#define __SWI_IN_OUT(NUM, INPUT, OUTPUT)                                       \
  __asm__ volatile("mov r0, %2\n\tswi %1\n\tmov %0, r0"                        \
                   : OUTPUT                                                    \
                   : "I"(NUM), INPUT)
#define __SWI_OUT(NUM, OUTPUT)                                                 \
  __asm__ volatile("swi %1\n\tmov %0, r0" : OUTPUT : "I"(NUM))
#define __GET_SWI_MACRO(_1, _2, _3, NAME, ...) NAME
#define NUMBERED_SWI(...)                                                      \
  __GET_SWI_MACRO(__VA_ARGS__, __SWI_IN_OUT, __SWI_IN, __SWI)(__VA_ARGS__)

void sys_call_handler(unsigned int number, void *context) {
  unsigned int *registers = thread_registers_from_context(context);

  switch (number) {
  case SYSCALL_NUM_THREAD_EXIT:
    thread_finish();
    scheduler_next(context);
    break;
  case SYSCALL_NUM_THREAD_CREATE:
    registers[0] =
        thread_create((int (*)(void *))registers[0], (void *)registers[1]);
    break;
  case SYSCALL_NUM_THREAD_SLEEP:
    thread_sleep(registers[0]);
    scheduler_next(context);
    break;
  case SYSCALL_NUM_IO_READ_CHAR:
    // check if dbgu already has a next char to avoid unnecessary waiting
    if (dbgu_has_next()) {
      registers[0] = dbgu_getc();
    } else {
      // register the thread to wait for the next char
      thread_wait(RESOURCE_DBGU_RECEIVE);

      // switch current thread for the next ready one
      scheduler_next(context);
    }
    break;
  case SYSCALL_NUM_IO_PUT_CHAR:
    dbgu_putc(registers[0]);
    break;
  default:
    print("There is currently no handler defined for SWI #%d\n\r", number);
  }
}

void sys_call_post_unblock(enum resource_type blocking_resource,
                           unsigned int unblocked_thread_id) {
  void *context = thread_get_context(unblocked_thread_id);
  unsigned int *registers = thread_registers_from_context(context);

  switch (blocking_resource) {
  case RESOURCE_NONE:
    break;
  case RESOURCE_DBGU_RECEIVE:
    registers[0] = dbgu_getc();
    break;
  case RESOURCE_DBGU_TRANSMIT:
    break;
  case RESOURCE_WAITING_TIME:
    break;
  }
}

void sys_call_exit_thread() { NUMBERED_SWI(SYSCALL_NUM_THREAD_EXIT); }

int sys_call_create_thread(int (*fun)(void *), void *input) {
  int result;
  __asm__ volatile("mov r0, %2\n\tmov r1, %3\n\tswi %1\n\tmov %0, r0"
                   : "=r"(result)
                   : "I"(SYSCALL_NUM_THREAD_CREATE), "r"(fun), "r"(input));
  return result;
}

void sys_call_sleep(unsigned int duration) {
  __SWI_IN(SYSCALL_NUM_THREAD_SLEEP, "r"(duration));
}

char sys_call_read_char() {
  char c;
  __SWI_OUT(SYSCALL_NUM_THREAD_CREATE, "=r"(c));
  return c;
}

void sys_call_put_char(char c) { __SWI_IN(SYSCALL_NUM_IO_PUT_CHAR, "r"(c)); }

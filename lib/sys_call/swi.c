#include <sys_call.h>

// some macro magic with variable arguments
// inspired by https://stackoverflow.com/a/11763277
//
// This will only insert the SWI number if nothing else is given.
// If input and output are given, other macros are used.
// This only allows for one input and one ouput each, as otherwise the macro
// has to be more complex
#define __SWI(NUM) __asm__ volatile("swi %0" : : "I"(NUM) : "r14")
#define __SWI_IN(NUM, INPUT)                                                   \
  __asm__ volatile("mov r0, %1\n\tswi %0" : : "I"(NUM), INPUT : "r14")
#define __SWI_IN_OUT(NUM, INPUT, OUTPUT)                                       \
  __asm__ volatile("mov r0, %2\n\tswi %1\n\tmov %0, r0"                        \
                   : OUTPUT                                                    \
                   : "I"(NUM), INPUT                                           \
                   : "r14")
#define __SWI_OUT(NUM, OUTPUT)                                                 \
  __asm__ volatile("swi %1\n\tmov %0, r0" : OUTPUT : "I"(NUM) : "r14")
#define __GET_SWI_MACRO(_1, _2, _3, NAME, ...) NAME
#define NUMBERED_SWI(...)                                                      \
  __GET_SWI_MACRO(__VA_ARGS__, __SWI_IN_OUT, __SWI_IN, __SWI)(__VA_ARGS__)

void sys_call_exit_thread() { NUMBERED_SWI(SYSCALL_NUM_THREAD_EXIT); }

int sys_call_create_thread(int (*fun)(void *), void *input) {
  int result;
  __asm__ volatile("mov r0, %2\n\t"
                   "mov r1, %3\n\t"
                   "swi %1\n\t"
                   "mov %0, r0"
                   : "=r"(result)
                   : "I"(SYSCALL_NUM_THREAD_CREATE), "r"(fun), "r"(input)
                   : "r14");
  return result;
}

void sys_call_sleep(unsigned int duration) {
  __SWI_IN(SYSCALL_NUM_THREAD_SLEEP, "r"(duration));
}

char sys_call_read_char() {
  char c;
  __SWI_OUT(SYSCALL_NUM_IO_READ_CHAR, "=r"(c));
  return c;
}

void sys_call_put_char(char c) { __SWI_IN(SYSCALL_NUM_IO_PUT_CHAR, "r"(c)); }

void sys_call_register_irq_callback(unsigned int callback_id,
                                    int (*fun)(void *)) {
  __asm__ volatile("mov r0, %1\n\t"
                   "mov r1, %2\n\t"
                   "swi %0\n\t"
                   :
                   : "I"(SYSCALL_NUM_REGISTER_IRQ_CALLBACK), "r"(callback_id),
                     "r"(fun)
                   : "r14");
}

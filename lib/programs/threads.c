#include <print.h>
#include <shrine_os_lib.h>
#include <system.h>
#include <thread.h>
#include <util.h>

// this kind of closure will only work in gcc
int print_char_repeatedly(void *input) {
  const char x = *(char *)input;

  // print the character 6 times with small pauses
  volatile int c = 6;
  while (c-- > 0) {
    print("%c", x);
    volatile int i;
    for (i = 0; i < 90000000; i++) {
    }
  }

  return 0;
}

static char thread_char_buffer[MAX_NUM_THREADS];
static int started_threads_counter = 0;

void create_char_print_thread() {
  char c = sys_call_read_char();

  int create_result = thread_create(
      print_char_repeatedly, thread_char_buffer + started_threads_counter);
  if (create_result == 0) {
    thread_char_buffer[started_threads_counter % MAX_NUM_THREADS] = c;
    started_threads_counter++;
  }
}

void schedule_on_timer_irq(thread_context *context) { print("!"); }

/**
 * ATTENTION: In the task for this example it was asked to print a newline
 * for every context switch. Since u05, this was turned of in the
 * scheduler_next() function. We didnt implement any other way to print the
 * newline without lots of restructuring of code. Also since in u05 syscalls are
 * used and the whole interrupt initialization is moved to the OS setup, we now
 * use syscalls here now too. Check out thre `u04` tag to see how the code
 * before the change.
 */
int thread_program() {
  int intervall = get_number(
      "Enter the intervall of thread switches [in ms] and press ENTER", 1000);
  print("\n\rYou chose a switch intervall of %d ms\n\r", intervall);

  // install routines for specific interrupt types
  sys_call_register_irq_callback(DBGU_RECEIVE_HANDLER,
                                 create_char_print_thread);
  sys_call_register_irq_callback(SYSTEM_TIMER_HANDLER, schedule_on_timer_irq);

  // enable system timer interrupts and set time
  sys_call_st_set_pits_intervall(intervall);

  print("Type characters to start new threads that take turns to print the "
        "typed characters!\n\r");

  return 0;
}

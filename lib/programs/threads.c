#include "system.h"
#include <dbgu.h>
#include <mem_layout.h>
#include <print.h>
#include <scheduler.h>
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

char *const thread_char_buffer =
    (char *)_INTERNAL_DBGU_THREAD_INPUT_BUFFER_START;

static int started_threads_counter = 0;

void create_char_print_thread() {
  char c = dbgu_grab_char();

  int create_result = thread_create(
      print_char_repeatedly, thread_char_buffer + started_threads_counter);
  if (create_result == 0) {
    thread_char_buffer[started_threads_counter % MAX_NUM_THREADS] = c;
    started_threads_counter++;
  }
}

int idling() {
  int c = 0;
  while (1) {
    print("idle... (%d)   \r", c++);
    volatile int i;
    for (i = 0; i < 300000000; i++) {
    }
  }
}

void schedule_on_timer_irq(void *context) {
  print("!");
  scheduler_next(context);
}

int thread_program() {
  int intervall = get_number(
      "Enter the intervall of thread switches [in ms] and press ENTER", 1000);
  print("\n\rYou chose a switch intervall of %d ms\n\r", intervall);

  // initialize the dbgu and enable its interrupts
  dbgu_initialize();

  // install handler for system interrupts (dbgu and system timer, currently)
  init_sys_interrupts();

  // install routines for specific interrupt types
  register_interrupt_routines(DBGU_RECEIVE_HANDLER, create_char_print_thread);
  register_interrupt_routines(SYSTEM_TIMER_HANDLER, schedule_on_timer_irq);

  // initialize the scheduler
  scheduler_init(idling);

  // enable system timer interrupts and set time
  st_activate_pits(intervall);

  cpsr_enable_interrupts();

  print("Type characters to start new threads that take turns to print the "
        "typed characters!\n\r");

  return 0;
}

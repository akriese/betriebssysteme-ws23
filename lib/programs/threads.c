#include "system.h"
#include <dbgu.h>
#include <print.h>
#include <scheduler.h>
#include <thread.h>

// this kind of closure will only work in gcc
int (*print_char_repeatedly(char c))(void) {
  int inside(void) {
    // print the character endlessly
    volatile int c = 6;
    while (c-- > 0) {
      print("%c", c);
      volatile int i;
      for (i = 0; i < 1000000; i++) {
      }
    }

    return 0;
  }

  return inside;
}

int thread_spawner() {
  while (1) {
    char c = dbgu_getc();

    int create_result = thread_create(print_char_repeatedly(c));

    if (create_result == 1) {
      print("Max number of threads reached, cant create another!");
    } else {
      print("Thread with char %c started!\n\r", c);
    }
  }

  return 0;
}

int idling() {
  print("idle thread started");
  while (1) {
    int i;
    for (i = 0; i < 100000; i++) {
      print("idle");
    }
  }
}

int str_to_int(char *s) {
  int res = 0;
  while (*s) {
    res *= 10;
    res += (*s - '0');
    s++;
  }

  return res;
}

int thread_program() {
  // enable interrupts
  // enable dbug interrupts
  // enable interrupts in the cpu
  cpsr_enable_interrupts();
  print("Enter the intervall of thread switches [in ms] and press ENTER: > ");
  char number_string[6];
  unsigned int digit_counter = 0;
  do {
    char c = dbgu_grab_char();
    if (c == 13 || c == 10) {
      number_string[digit_counter] = '\0';
      break;
    } else if (c == 0x7f) { // backspace
      // overwrite last char with space and move cursor back
      print("%c %c", 8, 8);
      digit_counter--;
    } else {
      number_string[digit_counter++] = c;
      print("%c", c);
    }
  } while (1);

  int intervall = str_to_int(number_string);
  print("\n\rYou chose a switch intervall of %d ms\n\r", intervall);

  // initialize the dbgu and enable its interrupts
  dbgu_initialize();

  // install handler for system interrupts (dbgu and system timer, currently)
  install_interrupt_handlers();

  // initialize the scheduler
  scheduler_init(idling);

  // enable system timer interrupts and set time
  st_activate_pits(intervall);

  print("Type characters to start new threads that take turns to print the "
        "typed characters!\n\r");

  while (1) {
  }

  return 0;
}

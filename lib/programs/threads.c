#include "system.h"
#include <dbgu.h>
#include <print.h>
#include <scheduler.h>
#include <thread.h>

int idling() {
  int c = 0;
  while (1) {
    print("idle... (%d)   \r", c++);
    volatile int i;
    for (i = 0; i < 300000000; i++) {
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
  print("Enter the intervall of thread switches [in ms] and press ENTER "
        "[default: 1000]: > ");
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

  int intervall = digit_counter == 0 ? 1000 : str_to_int(number_string);
  print("\n\rYou chose a switch intervall of %d ms\n\r", intervall);

  // initialize the dbgu and enable its interrupts
  dbgu_initialize();

  // install handler for system interrupts (dbgu and system timer, currently)
  install_interrupt_handlers();

  // initialize the scheduler
  scheduler_init(idling);

  // enable system timer interrupts and set time
  st_activate_pits(intervall);

  cpsr_enable_interrupts();

  print("Type characters to start new threads that take turns to print the "
        "typed characters!\n\r");

  return 0;
}

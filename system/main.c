// include what you want to execute
#include "sys_call.h"
#include <dbgu.h>
#include <example_programs.h>
#include <print.h>
#include <system.h>
#include <util.h>

#define N_PROGRAMS 5

int main_thread(void *__unused) {
  print("Welcome to shrineOS!\n\n\r");
  print("Here are some example programs to run:\n\r");

  const char *descriptions[N_PROGRAMS] = {
      "Test in and output of the DBGU! (u01)",
      "Try out different exceptions! (u02)",
      "See system timer and dbgu interrupts in action! (u03)",
      "Start threads and let them take turns! (u04)",
      "Start threads that use sys calls for kernel ops! (u05)"};

  int i;
  for (i = 0; i < N_PROGRAMS; ++i) {
    print("%d: %s\n\r", i + 1, descriptions[i]);
  }

  int choice = get_number("Pick an example procedure to run", N_PROGRAMS);

  switch (choice) {
  case (1):
    return dbgu_program();
    break;
  case (2):
    return exception_program();
    break;
  case (3):
    return interrupt_program();
    break;
  case (4):
    return thread_program();
    break;
  case (5):
    // falling through to default

  default:
    if (choice != N_PROGRAMS)
      print("Defaulting to %d\n\r", N_PROGRAMS);
    return sys_call_application();
  }

  sys_call_exit_thread();
  return 1;
}

int main() {
  // start main os thread
  sys_call_create_thread(main_thread, 0);

  while (1) {
  }
}

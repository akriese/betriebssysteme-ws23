// include what you want to execute
#include <dbgu.h>
#include <example_programs.h>
#include <print.h>
#include <system.h>
#include <util.h>

#define N_PROGRAMS 5

int main() {
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

  dbgu_enable();

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
    thread_program();
  case (5):
    // falling through to default

  default:
    if (choice != N_PROGRAMS)
      print("Defaulting to %d\n\r", N_PROGRAMS);
    return sys_call_application();
  }

  return 1;
}

// include what you want to execute
#include <dbgu.h>
#include <example_programs.h>
#include <print.h>
#include <system.h>

#define N_PROGRAMS 4

int main() {
  print("Welcome to shrineOS!\n\n\r");
  print("Pick an example procedure to run[default: %d]:\n\r", N_PROGRAMS);

  const char *descriptions[N_PROGRAMS] = {
      "Test in and output of the DBGU! (u01)",
      "Try out different exceptions! (u02)",
      "See system timer and dbgu interrupts in action! (u03)",
      "Start threads and let them take turns! (u04)"};

  int i;
  for (i = 0; i < N_PROGRAMS; ++i) {
    print("%d: %s\n\r", i + 1, descriptions[i]);
  }

  dbgu_enable();

  char c = dbgu_grab_char();
  print("You chose: %c\n\r", c);

  switch (c) {
  case ('1'):
    print("test");
    return dbgu_program();
    break;
  case ('2'):
    return exception_program();
    break;
  case ('3'):
    // falling through to default

  default:
    return interrupt_program();
  }

  return 1;
}

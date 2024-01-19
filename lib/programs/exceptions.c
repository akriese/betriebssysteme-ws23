#include <example_programs.h>
#include <print.h>
#include <shrine_os_lib.h>

/**
 * @brief Triggers different CPU exceptions on certain keypresses.
 *
 * ATTENTION: This program was first introduced in `u02`. Since then, the OS has
 * undergone some changes which broke it (interrupts, syscalls, etc.).
 * Thus, the program has been altered to accomodate for those changes.
 * Check out the git tag `u02` to see the actual implementation.
 */
int exception_program() {
  print("Press the following keys for special actions:\n\r");
  print("q - exit this program\n\r");
  print("U - undefined instruction\n\r");
  print("P - prefetch abort (not implemented)\n\r");
  print("D - data abort (not implemented)\n\r");
  print("S - software interrupt (not implemented)\n\r");

  for (;;) {
    char c = sys_call_read_char();
    print("You gave the char: %c (%x)\n\r", c, c);

    int break_loop = 0;

    switch (c) {
    case 'U':
      asm("udf #0");
      break;
    case 'P':
      // prefetch abort
      break;
    case 'D':
      *(int *)0xa0000000 = 0;
      // data abort
      break;
    case 'S':
      // software interrupt
      asm("swi #0");
      break;
    case 'q':
      print("And now, I'll terminate myself...\n\r");
      break_loop = 1;
      break;
    default:
      break;
    }

    if (break_loop)
      break;
  }

  return 0;
}

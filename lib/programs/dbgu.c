#include <print.h>
#include <shrine_os_lib.h>

/**
 * @brief Simple program that repeatedly reads a char and prints it with hex
 * code.
 *
 * ATTENTION: This program was first introduced in `u01`. Since then, the OS has
 * undergone some changes which broke it (interrupts, syscalls, etc.).
 * Thus, the program has been altered to accomodate for those changes.
 * Check out the git tag `u01` to see the actual implementation.
 */
int dbgu_program() {
  print("Type a character and see, what the OS responds ;)\n\r");

  while (1) {
    char c = sys_call_read_char();

    print("You typed the character %c (%x)\n\r", c, c);

    if (c == 'q') {
      break;
    }
  }

  return 0;
}

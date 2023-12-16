#include <dbgu.h>
#include <print.h>

int dbgu_program() {
  // dbgu_enable();
  print("Type a character and see, what the OS responds ;)\n\r");

  while (1) {
    char c = dbgu_grab_char();

    print("You typed the character %c (%x)\n\r", c, c);

    if (c == 'q') {
      break;
    }
  }

  return 0;
}

#include <dbgu.h>
#include <print.h>
#include <util.h>

void register_dump(unsigned int registers[16]) {
  print("REGISTER DUMP START\n\r");

  unsigned int const *r = registers;

  int i;
  for (i = 0; i < 3; i++) {
    int idx = i * 4;
    print("r%d:\t %X;\tr%d:\t %X;\tr%d:\t %X;\tr%d:\t %X\n\r", idx, r[idx],
          idx + 1, r[idx + 1], idx + 2, r[idx + 2], idx + 3, r[idx + 3]);
  }

  // adding 4 to sp as pc has to be pushed manually before function call
  print("r12:\t %X;\tsp:\t %X;\tlr:\t %X;\tpc:\t %X\n\r", r[12], r[13], r[14],
        r[15]);
  print("REGISTER DUMP END\n\r");
}

/**
  * Example usage:
        mov r0, #13
        mov r1, sp
        bl print_single_register
  *
  */
void print_single_register(int reg, int value) {
  char *regName;
  switch (reg) {
  case 13:
    regName = "sp";
    break;
  case 14:
    regName = "lr";
    break;
  case 15:
    regName = "pc";
    break;
  default:
    regName = "";
  }

  if (regName[0] != '\0') {
    print("%s: %x\n\r", regName, value);
  } else {
    print("r%d: %x\n\r", reg, value);
  }
}

int _print_mem(int address, int content) {
  print("%x: %x\n\r", address, content);
  return 0;
}

int _print_exception(int type) {
  char *text;

  switch (type) {
  case 0:
    text = "reset";
    break;
  case 1:
    text = "undefined instruction";
    break;
  case 2:
    text = "software interrupt";
    break;
  case 3:
    text = "prefetch abort";
    break;
  case 4:
    text = "data abort";
    break;
  case 5:
    text = "irq";
    break;
  case 6:
    text = "fiq";
    break;
  default:
    text = "UNKNOWN";
  }

  print("Exception triggered: %s\n\r", text);
  return 0;
}

/**
 * @brief Writes characters into the given buffer until a carriage return.
 *
 * If the input exceeds the max_length parameter, the function returns -1.
 * The user then has to call the function another time after working with the
 * buffer contents.
 *
 * @param buffer A char buffer which the functions writes to.
 * @param max_length The length of the buffer.
 * @return Success number. 0 on success, 1 on empty input (0th char is \n),
 * -1 on buffer overflow
 */
int get_line(char *buffer, unsigned int max_length) {
  unsigned int counter = 0;
  do {
    if (counter == max_length - 1) {
      return -1;
    }

    char c = dbgu_grab_char();
    if (c == 13 || c == 10) {
      buffer[counter] = '\0';
      print("\n\r");
      break;
    } else if (c == 0x7f) { // backspace
      if (counter == 0) {
        continue;
      }
      // overwrite last char with space and move cursor back
      print("%c %c", 8, 8);
      counter--;
    } else {
      buffer[counter++] = c;
      print("%c", c);
    }
  } while (1);

  if (counter == 0) {
    return 1;
  }

  return 0;
}

/**
 * @brief Parse a string to a signed integer.
 *
 * @param s String to be parsed.
 * @return Parsed number (can be negative)
 */
int str_to_int(char *s) {
  int is_negative = *s == '-';

  if (is_negative)
    s++;

  int res = 0;
  while (*s) {
    res *= 10;
    res += (*s - '0');
    s++;
  }

  return is_negative ? -res : res;
}

/**
 * @brief Parses input from the dbgu as a number.
 *
 * @param prompt String that is used to prompt the user for a number.
 * @param default_number Default to use if input is empty or otherwise invalid.
 * @return The input number or default if not correctly parseable.
 */
int get_number(char *prompt, int default_number) {
  print("%s [%d] > ", prompt, default_number);

  const unsigned int number_digits = 12;
  char number_input_buffer[number_digits + 1];
  int success = get_line(number_input_buffer, number_digits);

  int number;
  if (success == -1) {
    number_input_buffer[number_digits] = '\0';
    number = str_to_int(number_input_buffer);

    print("WARNING: Number too long. Taking the first %d digits (%d)",
          number_digits, number);
  } else if (success == 1) {
    number = default_number;
  } else {
    number = str_to_int(number_input_buffer);
  }

  return number;
}

/**
 * @brief checks if the given char is upper case
 *
 * @param c character to be checked.
 * @return 1 if upper case, 0 if not
 */
int is_upper(char c) { return c >= 'A' && c <= 'Z'; }

/**
 * @brief checks if the given char is lower case
 *
 * @param c character to be checked.
 * @return 1 if lower case, 0 if not
 */
int is_lower(char c) { return is_upper(c - ('a' - 'A')); }

/**
 * @brief Checks if the given char is a digit.
 *
 * @param c Character to be checked
 * @return 1 if char is digit, 0 otherwise.
 */
int is_digit(char c) { return c >= '0' && c <= '9'; }

/**
 * @brief A function that prints something infinitely with some time inbetween.
 */
int idling() {
  int c = 0;
  while (1) {
    // print("idle... (%d)   \r", c++);
    print(".");
    volatile int i;
    for (i = 0; i < 100000000; i++) {
    }
  }

  return 0;
}

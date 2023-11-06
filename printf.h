// prints a string with format identifiers and variable arguments
void printf(const char *fmt, ...);

// prints a string with format identifiers and variable arguments
// additionally appends a new line to write a line
void print_ln(const char *fmt, ...) {
  // print(fmt, ...);
  printf("\n\r");
}

#ifndef _DBGU_H_
#define _DBGU_H_

// Reads a char from the serial interface
char dbgu_getc();

// Writes a char to the serial interface
void dbgu_putc(char c);

// Writes a string to the serial interface. Stops at the first \0 in s
void serial_write_string(char *s);

#endif // !_DBGU_H_

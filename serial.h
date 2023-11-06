// Reads a char from the serial interface
char serial_read();

// Writes a char to the serial interface
void serial_write(char c);

// Writes a string to the serial interface. Stops at the first \0 in s
void serial_write_string(char *s);

// Reads a char from the serial interface
char read();

// Writes a char to the serial interface
void write(char c);

// Writes a string to the serial interface. Stops at the first \0 in s
void write_string(char *s);


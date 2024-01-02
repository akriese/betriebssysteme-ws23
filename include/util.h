void register_dump(unsigned int registers[16]);

void print_single_register(int reg, int value);

int _print_mem(int address, int content);

int _print_exception(int type);

int get_line(char *buffer, unsigned int max_length);

int str_to_int(char *s);

int get_number(char *prompt, int default_number);

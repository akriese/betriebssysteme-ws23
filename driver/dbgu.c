#include <data_structures.h>
#include <dbgu.h>
#include <mem_addresses.h>
#include <mem_layout.h>

#define RXEN_BIT 4  // enables read
#define RXDIS_BIT 5 // disables read
#define TXEN_BIT 6  // enables write
#define TXDIS_BIT 7 // disables write

#define RXRDY_BIT 0 // checking for available receive
#define TXRDY_BIT 1 // checking for possible transmit

#define IER_RXRDY_BIT 0

struct dbgu {
  unsigned int cr;
  unsigned int __mr;
  unsigned int ier;
  unsigned int idr;
  unsigned int imr;
  unsigned int sr;
  unsigned int rhr;
  unsigned int thr;
  unsigned int __brgr;
  unsigned int __reserved[(0x40 - 0x24) >> 2];
  unsigned int __cidr;
  unsigned int __exid;
};

volatile struct dbgu *const dbgu = (struct dbgu *)DBGU;

volatile struct ring_buffer *receive_buffer;

/*
 * Returns 1 if a bit at the given position at the given address is set.
 */
int check_status(unsigned int bit_position) {
  return dbgu->sr & 1 << bit_position;
}

/*
 * Sets a bit at the given position to 1.
 */
void set_status(unsigned int bit_position) { dbgu->cr = 1 << bit_position; }

/**
 * @brief Initialize the debug unit for receiving characters
 * and writing them to a ring buffer.
 */
void dbgu_initialize() {
  set_status(RXEN_BIT);
  receive_buffer =
      ring_buffer_create(_INTERNAL_DBGU_RECEIVE_BUFFER_SIZE,
                         (unsigned int *)_INTERNAL_DBGU_RECEIVE_BUFFER_START);

  dbgu_enable_interrupts();
}

void dbgu_enable() { set_status(RXEN_BIT); }

/*
 * Reads the char in the receive holding register.
 */
char read_char() { return dbgu->rhr; }

/*
 * Puts a given char into the transmit holding register.
 */
void write_char(char c) { dbgu->thr = c; }

/*
 * Wait for a char to be received and return that char.
 *
 * There is no need to clear the register or signal anything after reading,
 * as that is done by the controller upon the register read instruction.
 */
char dbgu_getc() {
  // enable receive controller

  // wait for new character in ring buffer
  unsigned int *c;
  do {
    c = ring_buffer_get(receive_buffer);
  } while (c == 0);

  return *(char *)c;
  // disable read
  // set_status(RXDIS_BIT);
}

/*
 * Put the given char into the transmit register.
 *
 * The controller has to be enabled before and disabled afterwards.
 */
void dbgu_putc(char c) {
  // enable write controller
  set_status(TXEN_BIT);

  // wait for permission to write another char
  while (!check_status(TXRDY_BIT)) {
  }

  write_char(c);

  // disable transmitter peacefully
  set_status(TXDIS_BIT);
}

/*
 * Send the given string via the transmitter.
 *
 * Always wait for the transmitter to be ready before settings the next char.
 *
 * The controller has to be enabled before the start of the string send and
 * disabled afterwards, but not inbetween, as multiple characters can be sent in
 * a row.
 */
void serial_write_string(char *s) {
  // enable write controller
  set_status(TXEN_BIT);

  while (*s) {
    while (!check_status(TXRDY_BIT)) {
    }

    write_char(*s);

    ++s;
  }

  // disable transmitter peacefully
  set_status(TXDIS_BIT);
}

void dbgu_enable_interrupts() { dbgu->ier = 1 << IER_RXRDY_BIT; }

int dbgu_interrupt_active() { return check_status(RXRDY_BIT); }

void dbgu_receive_interrupt_handler() {
  char c = read_char();
  ring_buffer_put(receive_buffer, (unsigned int)c);
}

char dbgu_grab_char() {
  while (!check_status(RXRDY_BIT)) {
  }
  return read_char();
}

// this kind of closure will only work in gcc
int print_char_repeatedly(void *input) {
  const char x = *(char *)input;

  // print the character 6 times with small pauses
  volatile int c = 6;
  while (c-- > 0) {
    print("%c", x);
    volatile int i;
    for (i = 0; i < 90000000; i++) {
    }
  }

  return 0;
}

char *const thread_char_buffer =
    (char *)_INTERNAL_DBGU_THREAD_INPUT_BUFFER_START;

static int started_threads_counter = 0;

void dbgu_create_thread_on_interrupt() {
  char c = read_char();

  int create_result = thread_create(
      print_char_repeatedly, thread_char_buffer + started_threads_counter);
  if (create_result == 0) {
    thread_char_buffer[started_threads_counter % MAX_NUM_THREADS] = c;
    started_threads_counter++;
  }
}

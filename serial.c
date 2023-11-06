#include "serial.h"

#define DBGU 0xFFFFF200 // Address of the debug unit's controller mapping

#define DBGU_CR (DBGU | 0x0000) // Debug controller control register
#define RXEN_BIT 4              // enables read
#define RXDIS_BIT 5             // disables read
#define TXEN_BIT 6              // enables write
#define TXDIS_BIT 7             // disables write

#define DBGU_SR (DBGU | 0x0014) // Debug controller status register
#define RXRDY_BIT 0             // checking for available receive
#define TXRDY_BIT 1             // checking for possible transmit

#define DBGU_RHR (DBGU | 0x18) // Receive holding register
#define DBGU_THR (DBGU | 0x1c) // Transmit holding register

/*
 * Returns 1 if a bit at the given position at the given address is set.
 */
int check_status(unsigned int address, unsigned int bit_position) {
  return *(volatile unsigned int *)address & 1 << bit_position;
}

/*
 * Sets a bit at the given position to 1.
 */
void set_status(unsigned int address, unsigned int bit_position) {
  *(volatile unsigned int *)address = 1 << bit_position;
}

/*
 * Reads the char in the receive holding register.
 */
char read_char() { return *(volatile char *)DBGU_RHR; }

/*
 * Puts a given char into the transmit holding register.
 */
void write_char(char c) { *(volatile char *)DBGU_THR = c; }

/*
 * Wait for a char to be received and return that char.
 *
 * There is no need to clear the register or signal anything after reading,
 * as that is done by the controller upon the register read instruction.
 */
char serial_read() {
  // enable receive controller
  set_status(DBGU_CR, RXEN_BIT);

  // wait for incoming character
  while (!check_status(DBGU_SR, RXRDY_BIT)) {
  }

  char input = read_char();

  // disable read
  set_status(DBGU_CR, RXDIS_BIT);

  return input;
}

/*
 * Put the given char into the transmit register.
 *
 * The controller has to be enabled before and disabled afterwards.
 */
void serial_write(char c) {
  // enable write controller
  set_status(DBGU_CR, TXEN_BIT);

  // wait for permission to write another char
  while (!check_status(DBGU_SR, TXRDY_BIT)) {
  }

  write_char(c);

  // disable transmitter peacefully
  set_status(DBGU_CR, TXDIS_BIT);
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
  set_status(DBGU_CR, TXEN_BIT);

  while (*s) {
    while (!check_status(DBGU_SR, TXRDY_BIT)) {
    }

    write_char(*s);

    ++s;
  }

  // disable transmitter peacefully
  set_status(DBGU_CR, TXDIS_BIT);
}

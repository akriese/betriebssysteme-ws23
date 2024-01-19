#include <mem_addresses.h>
#include <print.h>

typedef struct system_timer {
  unsigned int cr;
  unsigned int pimr;
  unsigned int __wdmr;
  unsigned int __rtmr;
  unsigned int sr;
  unsigned int ier;
  unsigned int idr;
  unsigned int imr;
  unsigned int __rtar;
  unsigned int __crtr;
} system_timer;

volatile system_timer *const st = (system_timer *)SYSTEM_TIMER;

/**
 * @brief Converts milliseconds to the system timer intervall.
 * This assumes a timer frequency of 32,768 herz.
 *
 * @param ms Duration in milliseconds.
 * @return Timer intervall that corresponds to the given milliseconds.
 */
unsigned int ms_to_intervall(unsigned short ms) { return 32767 * ms / 1000; }

/**
 * @brief Backwards calculation of ms_to_intervall().
 *
 * @param intervall Timer intervall (max: 32767)
 * @return Intervall in ms.
 */
unsigned int intervall_to_ms(unsigned int intervall) {
  return intervall * 1000 / 32767;
}

void st_set_intervall(unsigned int ms) {
  unsigned int intervall = ms_to_intervall(ms);
  if (intervall > 0xffff) {
    print("Reducing the intervall down to %d (max ms=2000)\n\r", 0xffff);
    intervall = 0xffff;
  }
  st->pimr = intervall; // set the time
}

/*
 * Time can be a maximum clock cycles of 65535.
 *
 * As the clock speed is about 32kHz, this would be 2 seconds.
 *
 */
void st_activate_pits() { st->ier = 1 << 0; }

/**
 * @brief Checks if the PITS has an active interrupt.
 *
 * @return 1 if active, 0 if not
 */
int st_interrupt_active() { return st->sr & 1 << 0; }

/**
 * @brief Gets the current intervall of the PITS in ms.
 *
 * @return The intervall in ms.
 */
int st_get_intervall() { return intervall_to_ms(st->pimr); }

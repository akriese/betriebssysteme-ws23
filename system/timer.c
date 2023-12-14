#include <mem_addresses.h>

struct system_timer {
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
};

volatile struct system_timer *const system_timer =
    (struct system_timer *)SYSTEM_TIMER;

/**
 * @brief Converts milliseconds to the system timer intervall.
 * This assumes a timer frequency of 32,768 herz.
 *
 * @param ms Duration in milliseconds.
 * @return Timer intervall that corresponds to the given milliseconds.
 */
unsigned int ms_to_intervall(unsigned short ms) { return 32768 * ms / 1000; }

/*
 * Time can be a maximum clock cycles of 65536.
 *
 * As the clock speed is about 32kHz, this would be 2 seconds.
 *
 */
void st_activate_pits(unsigned short ms) {
  system_timer->pimr = ms_to_intervall(ms); // set the time
  system_timer->ier = 1 << 0; // set the bit for the PITS interrupt
}

int st_interrupt_active() { return system_timer->sr & 1 << 0; }

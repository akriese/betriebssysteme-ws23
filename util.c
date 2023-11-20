#include "printf.h"

void register_dump(int r0, int r1, int r2, int r3, int r4, int r5, int r6,
                   int r7, int r8, int r9, int r10, int r11, int sp, int pc,
                   int lr) {
  printf("REGISTER DUMP START\n\r");
  printf("r0: %x;\tr1: %x;\tr2: %x\n\r", r0, r1, r2);
  printf("r3: %x;\tr4: %x;\tr5: %x\n\r", r3, r4, r5);
  printf("r6: %x;\tr7: %x;\tr8: %x\n\r", r6, r7, r8);
  printf("r9: %x;\tr10: %x;\tr11: %x\n\r", r9, r10, r11);
  printf("sp: %x;\tpc: %x;\tlr: %x\n\r", sp, pc, lr);
  printf("REGISTER DUMP END\n\r");
}

int _print_mem(int address, int content) {
  printf("%x: %x\n\r", address, content);
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

  printf("Exception triggered: %s\n\r", text);
  return 0;
}

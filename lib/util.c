#include <print.h>

void register_dump(int r0, int r1, int r2, int r3, int r4, int r5, int r6,
                   int r7, int r8, int r9, int r10, int r11, int r12, int sp,
                   int lr, int pc) {
  print("REGISTER DUMP START\n\r");
  print("r0: %x;\tr1: %x;\tr2: %x\n\r", r0, r1, r2);
  print("r3: %x;\tr4: %x;\tr5: %x\n\r", r3, r4, r5);
  print("r6: %x;\tr7: %x;\tr8: %x\n\r", r6, r7, r8);
  print("r9: %x;\tr10: %x;\tr11: %x\n\r", r9, r10, r11);
  print("r12: %x;\n\r", r12);

  // adding 4 to sp as pc has to be pushed manually before function call
  print("sp: %x;\tlr: %x;\tpc: %x\n\r", sp + 4, lr, pc);
  print("REGISTER DUMP END\n\r");
}

/**
  * Example usage:
        mov r0, #13
        mov r1, sp
        bl print_single_register
  *
  */
void print_single_register(int reg, int value) {
  char *regName;
  switch (reg) {
  case 13:
    regName = "sp";
    break;
  case 14:
    regName = "lr";
    break;
  case 15:
    regName = "pc";
    break;
  default:
    regName = "";
  }

  if (regName[0] != '\0') {
    print("%s: %x\n\r", regName, value);
  } else {
    print("r%d: %x\n\r", reg, value);
  }
}

int _print_mem(int address, int content) {
  print("%x: %x\n\r", address, content);
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

  print("Exception triggered: %s\n\r", text);
  return 0;
}

// include what you want to execute
#include <dbgu.h>
#include <print.h>
#include <system.h>

extern int _perform_reset(void);
extern int _perform_int(void);

extern void *check_stack_pointer(enum cpu_mode mode);

int interrupt_program() {
  print("start of the program\n\r");
  dbgu_enable_interrupt();
  st_activate_pits((unsigned char)1000);
  print("IRQ stack pointer: %p\n\r", check_stack_pointer(CPU_MODE_IRQ));
  print("SYS stack pointer: %p\n\r", check_stack_pointer(CPU_MODE_SYS));
  print("UND stack pointer: %p\n\r", check_stack_pointer(CPU_MODE_UND));

  install_interrupt_handlers();

  print("setup done!\n\r");

  while (1) {
    dbgu_getc();

    volatile int i; // volatile so it is not optimized away
    for (i = 0; i < 10000; i++) {
      // do nothing
    }
  }

  return 0;
}

int exception_program() {
  char *intro =
      "Welcome to shrineOS! Type a character and shrineOS will respond.";
  print("\n\r%s (string stored at %x)\n\r", intro, intro);
  print("Press the following keys for special actions:\n\r");
  print("q - exit this program\n\r");
  print("Q - system reset\n\r");
  print("I - interrupt\n\r");
  print("U - undefined instruction\n\r");
  print("P - prefetch abort (not implemented)\n\r");
  print("D - data abort (not implemented)\n\r");
  print("S - software interrupt (not implemented)\n\r");

  for (;;) {
    char c = dbgu_getc();
    print("You gave the char: %c (%x)\n\r", c, c);

    int break_loop = 0;

    switch (c) {
    case 'Q':
      _perform_reset();
      break;
    case 'I':
      _perform_int();
      break;
    case 'U':
      asm("udf #0");
      break;
    case 'P':
      // prefetch abort
      break;
    case 'D':
      *(int *)0xa0000000 = 0;
      // data abort
      break;
    case 'S':
      // software interrupt
      asm("swi #0");
      break;
    case 'q':
      print("And now, I'll terminate myself...\n\r");
      break_loop = 1;
      break;
    default:
      print("shrineOS says: %c\n\r", c + 1);
    }

    if (break_loop)
      break;
  }

  return 0;
}

int main() { return interrupt_program(); }

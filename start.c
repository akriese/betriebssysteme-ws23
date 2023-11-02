// include what you want to execute

__attribute__((naked, section(".init")))
void _start(void) {
  // execute your os here

  // then use an endless loop (for now)
  for(;;);
}

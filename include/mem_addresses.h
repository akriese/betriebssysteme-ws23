#define INTERNAL_RAM 0x00200000

#define DBGU 0xFFFFF200 // Address of the debug unit's controller mapping
#define DBGU_CR (DBGU | 0x0000) // Debug controller control register
#define DBGU_SR (DBGU | 0x0014) // Debug controller status register
#define DBGU_RHR (DBGU | 0x18)  // Receive holding register
#define DBGU_THR (DBGU | 0x1c)  // Transmit holding register

#define MEM_CONTROLLER_UI 0xFFFFFF00

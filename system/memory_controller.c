#include <mem_addresses.h>

void mc_remap() { *(volatile unsigned int *)MEM_CONTROLLER_UI = 1; }

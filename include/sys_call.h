#ifndef _SYS_CALL_H_
#define _SYS_CALL_H_

void sys_call_handler(unsigned int number, void *context);

void sys_call_exit_thread();

#endif // !_SYS_CALL_H_

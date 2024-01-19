#ifndef _SYS_CALL_H_
#define _SYS_CALL_H_

#include <resource.h>
#include <thread.h>

#define SYSCALL_NUM_THREAD_EXIT 1
#define SYSCALL_NUM_THREAD_CREATE 2
#define SYSCALL_NUM_THREAD_SLEEP 3
#define SYSCALL_NUM_IO_READ_CHAR 4
#define SYSCALL_NUM_IO_PUT_CHAR 5
#define SYSCALL_NUM_REGISTER_IRQ_CALLBACK 6
#define SYSCALL_NUM_ST_SET_PITS_INTERVALL 7
#define SYSCALL_NUM_SET_IDLE_FUNCTION 8

void sys_call_handler(unsigned int number, thread_context *context);
void sys_call_post_unblock(resource_type blocking_resource,
                           unsigned int unblocked_thread_id);

void sys_call_exit_thread();
int sys_call_create_thread(int (*fun)(void *), void *input);
void sys_call_sleep(unsigned int duration);
char sys_call_read_char();
void sys_call_put_char(char c);
void sys_call_register_irq_callback(unsigned int callback_id,
                                    int (*fun)(void *));
void sys_call_st_set_pits_intervall(unsigned int ms);
void sys_call_set_idle_function(int (*idle_fun)());

#endif // !_SYS_CALL_H_

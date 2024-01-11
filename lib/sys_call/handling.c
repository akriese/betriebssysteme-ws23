#include <dbgu.h>
#include <print.h>
#include <scheduler.h>
#include <sys_call.h>
#include <system.h>
#include <thread.h>

/**
 * @brief Handles software interrupts which are currently only sys calls.
 *
 * Depending on the given SWI number, different routines are executed.
 *
 * @param number SWI number.
 * @param context Context of the currently running thread.
 */
void sys_call_handler(unsigned int number, thread_context *context) {
  unsigned int *registers = thread_registers_from_context(context);

  switch (number) {
  case SYSCALL_NUM_THREAD_EXIT:
    thread_finish();
    scheduler_next(context);
    break;
  case SYSCALL_NUM_THREAD_CREATE:
    registers[0] =
        thread_create((int (*)(void *))registers[0], (void *)registers[1]);
    break;
  case SYSCALL_NUM_THREAD_SLEEP:
    thread_sleep(registers[0]);
    scheduler_next(context);
    break;
  case SYSCALL_NUM_IO_READ_CHAR:
    // check if dbgu already has a next char to avoid unnecessary waiting
    if (dbgu_has_next()) {
      registers[0] = dbgu_getc();
    } else {
      // register the thread to wait for the next char
      thread_wait(RESOURCE_DBGU_RECEIVE);

      // switch current thread for the next ready one
      scheduler_next(context);
    }
    break;
  case SYSCALL_NUM_IO_PUT_CHAR:
    // r0 contains the character by convention of the syscall.
    dbgu_putc(registers[0]);
    break;
  case SYSCALL_NUM_REGISTER_IRQ_CALLBACK:
    register_interrupt_routines(registers[0], (void (*)(void *))registers[1]);
    break;
  default:
    print("There is currently no handler defined for SWI #%d\n\r", number);
  }
}

/**
 * @brief Handles actions specific to different blocking resources after they
 * were unblocked.
 *
 * Unblocked threads usually get some information after they are unblocked. This
 * is the place where this data transmission is handled.
 *
 * E.g. sys_call_read_char() suspends the reading thread until the DBGU sends a
 * receive IRQ. The blocked thread will be unblocked and the character is
 * written to r0 of the threads context, basically finishing the SWI call and
 * having the char as return value.
 *
 * @param blocking_resource The resource type that was unblocked.
 * @param unblocked_thread_id The thread that was unblocked.
 */
void sys_call_post_unblock(enum resource_type blocking_resource,
                           unsigned int unblocked_thread_id) {
  thread_context *context = thread_get_context(unblocked_thread_id);
  unsigned int *registers = thread_registers_from_context(context);

  switch (blocking_resource) {
  case RESOURCE_NONE:
    break;
  case RESOURCE_DBGU_RECEIVE:
    registers[0] = dbgu_getc();
    break;
  case RESOURCE_DBGU_TRANSMIT:
    break;
  case RESOURCE_WAITING_TIME:
    break;
  }
}

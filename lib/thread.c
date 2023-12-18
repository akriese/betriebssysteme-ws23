#include <mem.h>
#include <mem_layout.h>
#include <scheduler.h>
#include <thread.h>

volatile struct thread_control_block *const tcbs =
    (struct thread_control_block *)_INTERNAL_THREADS_TCB_ARRAY_START;

volatile struct thread_management *const management =
    (struct thread_management *)_INTERNAL_THREADS_MANAGEMENT_START;
/**
 * @brief Creates a new thread with which runs the given function.
 *
 * @return 0 on success, 1 on failure (e.g. no free TCB)
 */
int thread_create(/*function to run*/);

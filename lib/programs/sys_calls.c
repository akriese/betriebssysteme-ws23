#include <dbgu.h>
#include <mem_layout.h>
#include <print.h>
#include <scheduler.h>
#include <sys_call.h>
#include <system.h>
#include <thread.h>
#include <util.h>

struct print_thread_info {
  unsigned int sleep_or_repeat;
  char c;
};

struct print_thread_info thread_info_buffer[MAX_NUM_THREADS];

static int started_threads_counter = 0;
static int sleep_time = 500;
static int million_computation_cycles = 200;

int print_char_repeatedly_with_computation(void *input) {
  const struct print_thread_info *info = (struct print_thread_info *)input;
  const char x = info->c;
  const unsigned int compute_repititions = info->sleep_or_repeat;

  // print the character 6 times with small pauses
  volatile int c = 6;
  while (c-- > 0) {
    print("%c", x);
    volatile int i;
    for (i = 0; i < compute_repititions * 1000000; i++) {
    }
  }

  return 0;
}

int print_char_repeatedly_with_sleep(void *input) {
  const struct print_thread_info *info = (struct print_thread_info *)input;
  const char x = info->c;
  const unsigned int sleep_duration = info->sleep_or_repeat;

  // print the character 6 times sleeping a short time inbetween
  volatile int c = 6;
  while (c-- > 0) {
    print("%c", x);

    sys_call_sleep(sleep_duration);
  }

  return 0;
}

int input_callback(char c) {
  struct print_thread_info *thread_info =
      &thread_info_buffer[started_threads_counter % MAX_NUM_THREADS];

  char tmp_char = thread_info->c;
  char tmp_sleep_or_repeat = thread_info->sleep_or_repeat;

  thread_info->c = c;

  int create_result;

  // try to create the new thread
  if (is_upper(c)) {
    thread_info->sleep_or_repeat = million_computation_cycles;
    create_result = sys_call_create_thread(
        print_char_repeatedly_with_computation, thread_info);
  } else if (is_lower(c)) {
    thread_info->sleep_or_repeat = sleep_time;
    create_result =
        sys_call_create_thread(print_char_repeatedly_with_sleep, thread_info);
  } else {
    print("Not a valid character! Doing nothing!\n\r");
    return 1;
  }

  // if it didnt work out, reset the old value of the input holding array
  if (create_result != 0) {
    thread_info->c = tmp_char;
    thread_info->sleep_or_repeat = tmp_sleep_or_repeat;
  }

  started_threads_counter++;

  return 0;
}

int thread_starter(void *__unused) {
  // sys_call_register_irq_callback(DBGU_RECEIVE_HANDLER, input_callback);
  print("Type characters to start threads:\n\r"
        "- Upper case for actively waiting (computation)\n\r"
        "- Lower case for passively waiting (sleep)\n\r"
        "- ENTER for a linebreak\n\r\n\r");

  while (1) {
    char c = sys_call_read_char();

    // print newline if the user wants to have a line break
    if (c == 13 || c == 10) {
      print("\n\r");
      continue;
    }

    input_callback(c);
  }

  return 0;
}

int sys_call_application() {
  sleep_time =
      get_number("Enter the sleep duration [in ms] for threads!", sleep_time);
  million_computation_cycles = get_number(
      "Enter the number of computation cycles between prints (in millions)!",
      million_computation_cycles);
  int intervall = get_number(
      "Enter the intervall of thread switches [in ms] and press ENTER", 100);

  // initialize the dbgu and enable its interrupts
  dbgu_initialize();

  // install handler for system interrupts (dbgu and system timer, currently)
  init_sys_interrupts();

  // initialize the scheduler
  scheduler_init(idling);

  // enable system timer interrupts and set time
  st_activate_pits(intervall);

  cpsr_enable_interrupts();

  print("The application starts now!\n\r");

  sys_call_create_thread(thread_starter, 0);

  return 0;
}

#ifndef _DATA_STRUCTURES_H_
#define _DATA_STRUCTURES_H_

/**
 * @class ring_buffer
 * @brief Ring buffer for [unsigned int] with a length and two indices marking
 * current input and output pointers
 *
 * Expects that (length-3)*4 bytes are reserved (allocated) at *start.
 */
struct ring_buffer {
  unsigned int length;
  unsigned int next_in;
  unsigned int next_out;
  unsigned int *buffer;
};

volatile struct ring_buffer *ring_buffer_create(unsigned int length,
                                                unsigned int *start) {
  volatile struct ring_buffer *b = (struct ring_buffer *)start;
  b->length = length - 4;
  b->buffer = start + 4;
  b->next_in = 0;
  b->next_out = 0;

  return b;
}

void ring_buffer_put(volatile struct ring_buffer *b, unsigned int element) {
  b->buffer[b->next_in] = element;
  b->next_in = (b->next_in + 1) % b->length;
}

/**
 * Returns 0 if there is no new element in the buffer.
 * Otherwise returns the pointer to the object to get.
 */
unsigned int *ring_buffer_get(volatile struct ring_buffer *b) {
  if (b->next_out == b->next_in) {
    return 0;
  }

  unsigned int *element = b->buffer + b->next_out;
  b->next_out = (b->next_out + 1) % b->length;

  return element;
}

#endif /* ifndef _DATA_STRUCTURES_H_ */

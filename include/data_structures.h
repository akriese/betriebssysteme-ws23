#ifndef _DATA_STRUCTURES_H_
#define _DATA_STRUCTURES_H_

/**
 * Only for unsigned int currently
 */
struct ring_buffer {
  unsigned int length;
  unsigned int *next_in;
  unsigned int *next_out;
  unsigned int *buffer;
};

volatile struct ring_buffer *ring_buffer_create(unsigned int length,
                                                unsigned int *start) {
  volatile struct ring_buffer *b = (struct ring_buffer *)start;
  b->length = length - 3;
  b->buffer = start + 3;
  b->next_in = b->buffer;
  b->next_out = b->buffer;

  return b;
}

void ring_buffer_put(volatile struct ring_buffer *b, unsigned int element) {
  *(b->next_in) = element;
  b->next_in =
      ((unsigned int)b->next_in - (unsigned int)b->buffer + 1) % b->length +
      b->buffer;
}

/**
 * Returns 0 if there is no new element in the buffer.
 * Otherwise returns the pointer to the object to get.
 */
unsigned int *ring_buffer_get(volatile struct ring_buffer *b) {
  if (b->next_out == b->next_in) {
    return 0;
  }

  unsigned int *element = b->next_out;
  b->next_out =
      ((unsigned int)b->next_out - (unsigned int)b->buffer + 1) % b->length +
      b->buffer;

  return element;
}

#endif /* ifndef _DATA_STRUCTURES_H_ */

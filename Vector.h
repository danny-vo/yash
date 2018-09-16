#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdint.h>

typedef struct Vector {
  void** data;
  uint32_t length;
  uint32_t capacity;
  void (*deletor)(void*);
} Vector;

#endif /* VECTOR_H */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Vector.h"

#define MINIMUM_SIZE 0

Vector newVector(uint32_t initSize, void (*deletor)(void*)) {
  Vector vector;
  vector.length = initSize;
  vector.capacity = initSize < MINIMUM_SIZE ? MINIMUM_SIZE : initSize;
  vector.data = (void**)calloc(vector.capacity, sizeof(void*));
  vector.deletor = deletor ? deletor : free;
  return vector;
}

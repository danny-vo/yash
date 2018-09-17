#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Vector.h"

#define MINIMUM_SIZE 4

Vector* Vector_new(uint32_t initSize, void (*deletor)(void*), void (*printer)(void)) {
  Vector* vector = (Vector*)malloc(sizeof(Vector));
  vector->length = 0;
  vector->capacity = initSize < MINIMUM_SIZE ? MINIMUM_SIZE : initSize;
  vector->data = (void**)calloc(vector->capacity, sizeof(void*));
  vector->deletor = deletor ? deletor : free;
  vector->printer = printer;
  return vector;
}

void Vector_destroy(Vector* vector) {
  for (int i=0; i < vector->length; ++i) {
    if (NULL == vector->data[i]) { continue; }
    vector->deletor(vector->data[i]);
  }
  free(vector->data);
  free(vector);
}

void* Vector_getElem(Vector* vector, uint32_t idx) {
  return idx > vector->length ? NULL : vector->data[idx];
}

int Vector_setElem(Vector* vector, uint32_t idx, void* elem) {
  if (idx >= vector->length) {
    return 0;
  }
  vector->data[idx] = elem;
  return 1;
}

uint32_t Vector_size(Vector* vector) {
  return vector->length;
}

void** Vector_getData(Vector* vector) {
  return vector->data;
}

void* Vector_pop(Vector* vector) {
  return vector->length <=0 ? NULL : vector->data[--(vector->length)];
}

void Vector_push(Vector* vector, void* elem) {
  if (vector->length >= vector->capacity) {
    vector->capacity *= 2;
    void** newData = (void**)calloc(vector->capacity, sizeof(void*));
    for (uint32_t i=0; i < vector->length; ++i) {
      newData[i] = vector->data[i];
    }
    free(vector->data);
    vector->data = newData;
  }
  vector->data[(vector->length)++] = elem;
}

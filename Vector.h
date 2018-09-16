#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdint.h>

typedef struct Vector {
  void** data;
  uint32_t length;
  uint32_t capacity;
  void (*deletor)(void*);
  void (*printer)(void);
} Vector;

Vector* Vector_new(uint32_t initSize, void (*deletor)(void*), void (*printer)(void));
void Vector_destroy(Vector* vector);
void* Vector_getElem(Vector* vector, uint32_t idx);
int Vector_setElem(Vector* vector, uint32_t idx, void* elem);
uint32_t Vector_size(Vector* vector);
void** Vector_getData(Vector* vector);
void* Vector_pop(Vector* vector);
void Vector_push(Vector* vector, void* elem);

#endif /* VECTOR_H */

#ifndef _STRINGVECTOR_H
#define _STRINGVECTOR_H

#include <stdint.h>

typedef struct StringVector {
  char** data;
  uint32_t length;
  uint32_t capacity;
} StringVector;

StringVector createStrVec(uint32_t initVectorSize);
void destroyStrVec(StringVector);

char* getStrVecElem(StringVector, uint32_t idx);
void setStrVecElem(StringVector, uint32_t idx, char* str);
uint32_t strVecSize(StringVector strVec);
void strVecPopBack(StringVector strVec);
void strVecPushBack(StringVector* strVec, char* str);

#endif /* STRINGVECTOR_H */

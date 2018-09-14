#include <stdlib.h>

#include "StringVector.h"

#define MINIMUM_SIZE 4

StringVector createStrVec(uint32_t initVectorSize) {
  StringVector stringVector;

  stringVector.length = initVectorSize;
  stringVector.capacity = initVectorSize < MINIMUM_SIZE ? MINIMUM_SIZE : initVectorSize;

  stringVector.data = (char**)calloc(stringVector.capacity, sizeof(char));

  return stringVector;
}

void destroyStrVec(StringVector strVec) {
  for (int i=0; i < strVec.length; ++i) {
    free(strVec.data[i]);
  }
  free(strVec.data);
}

void checkRange(StringVector strVec, uint32_t idx) {
  if (idx >= strVec.length) {
    abort();
  }
}

char* getStrVecElem(StringVector strVec, uint32_t idx) {
  checkRange(strVec, idx);
  return strVec.data[idx];
}

void setStrVecElem(StringVector strVec, uint32_t idx, char* str) {
  checkRange(strVec, idx);
  strVec.data[idx] = str;
}

uint32_t strVecSize(StringVector strVec) {
  return strVec.length;
}

void strVecPopBack(StringVector strVec) {
  --strVec.length;
}

void strVecPushBack(StringVector* strVec, char* str) {
  if (strVec->length == strVec->capacity) {
    strVec->capacity *= 2;
    char** newData = (char**)calloc(strVec->capacity, sizeof(char*));
    for (uint32_t i=0; i < strVec->length; ++i) {
      newData[i] = strVec->data[i];
    }
    free(strVec->data);
    strVec->data = newData;
  }
  strVec->data[strVec->length] = str;
  strVec->length++;
}

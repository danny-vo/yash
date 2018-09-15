#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
  // Determine if we need to resize the vector
  if (strVec->length == strVec->capacity) {
    strVec->capacity *= 2;
    char** newData = (char**)calloc(strVec->capacity, sizeof(char*));
    for (uint32_t i=0; i < strVec->length; ++i) {
      newData[i] = strVec->data[i];
    }
    free(strVec->data);
    strVec->data = newData;
  }

  // Allocates the str argument on the heap
  int strLen = strlen(str)+1;
  char* heapStr = (char*)malloc((strLen+1)*sizeof(char));
  strcpy(heapStr, str);

  strVec->data[strVec->length] = heapStr;
  strVec->length++;
}

void strVecPrint(StringVector strVec) {
  printf("Vector length: %d\n", strVec.length);
  for (int i=0; i < strVec.length; ++i) {
    printf("%s\n", strVec.data[i]);
  }
}

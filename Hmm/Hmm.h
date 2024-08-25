#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "doubleLinked.h"

#define heapSize  300*1024*1024
#define blockSize 131072 
#define myceil(x) x - (unsigned long)x > 0 ? (unsigned long)(x+1) : (unsigned long)x


void *HmmAlloc(size_t size);
void HmmFree(void *ptr);
void *HmmCalloc(size_t nmemb, size_t size);
void *HmmReAlloc(void *pointer, size_t size);

#ifndef KMALLOC_H
#define KMALLOC_H

#include "stdint.h"

bool kmallocInit(uint32_t heapSize);
void changeHeapSize(int newSize);

#endif      // KMALLOC_H

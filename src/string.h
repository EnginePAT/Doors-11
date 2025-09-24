#ifndef STRING_H
#define STRING_H

#include "stdint.h"

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, unsigned int n);
int strtol(const char* str, char** endptr, int base);

#endif      // STRING_H

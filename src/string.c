#include "string.h"

// Compare two strings, returns 0 if equal
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Compare first n characters of two strings, returns 0 if equal
int strncmp(const char *s1, const char *s2, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
    }
    return 0;
}

// Convert null-terminated decimal string to integer
int strtol(const char* str, char** endptr, int base) {
    int result = 0;
    int sign = 1;

    // Handle optional sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str) {
        char c = *str;
        int digit;

        if (c >= '0' && c <= '9') digit = c - '0';
        else if (base == 16 && c >= 'a' && c <= 'f') digit = 10 + (c - 'a');
        else if (base == 16 && c >= 'A' && c <= 'F') digit = 10 + (c - 'A');
        else break; // invalid character

        if (digit >= base) break;

        result = result * base + digit;
        str++;
    }

    if (endptr) *endptr = (char*)str;
    return result * sign;
}

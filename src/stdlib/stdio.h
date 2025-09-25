#ifndef STDIO_H
#define STDIO_H

void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
int * printf_number(int*, int, bool, int);
void print_hex8(uint8_t val);
void print_hex16(uint16_t val);
void print_hex32(uint32_t val);
void print_dec(uint32_t num) ;

extern void x86_div64_32(uint64_t, uint32_t, uint64_t*, uint32_t*);

#define PRINTF_STATE_START 0
#define PRINTF_STATE_LENGTH 1
#define PRINTF_STATE_SHORT 2
#define PRINTF_STATE_LONG 3
#define PRINTF_STATE_SPEC 4

#define PRINTF_LENGTH_START 0
#define PRINTF_LENGTH_SHORT_SHORT 1
#define PRINTF_LENGTH_SHORT 2
#define PRINTF_LENGTH_LONG 3
#define PRINTF_LENGTH_LONG_LONG 4

#endif      // STDIO_H

#include "stdint.h"

void memset(void *dest, char val, uint32_t count);
void *memcpy(void *dest, const void *src, size_t n);
void outb(uint16_t Port, uint8_t Value);
char inb(uint16_t port);
void setCursor(int row, int col);

#define CEIL_DIV(a,b) (((a + b) - 1)/b)

struct InterruptRegisters{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;
};

static inline uint16_t inPortW(uint16_t port) {
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

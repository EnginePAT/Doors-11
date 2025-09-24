#include "util.h"

#define VGA_CTRL 0x3D4
#define VGA_DATA 0x3D5

void memset(void *dest, char val, uint32_t count){
    char *temp = (char*) dest;
    for (; count != 0; count --){
        *temp++ = val;
    }

}

// Copy `n` bytes from `src` to `dest`
void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void setCursor(int row, int col) {
    uint16_t pos = row * 80 + col;
    outb(VGA_CTRL, 0x0F);
    outb(VGA_DATA, (uint8_t)(pos & 0xFF));
    outb(VGA_CTRL, 0x0E);
    outb(VGA_DATA, (uint8_t)((pos >> 8) & 0xFF));
}

void outb(uint16_t Port, uint8_t Value) {
    asm volatile ("outb %1, %0" : : "dN" (Port), "a" (Value));
}

char inb(uint16_t port) {
    char rv;
    asm volatile("inb %1, %0": "=a"(rv):"dN"(port));
    return rv;
}

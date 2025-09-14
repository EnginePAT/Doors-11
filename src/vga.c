#include "vga.h"

uint16_t column = 0;
uint16_t line = 0;
uint16_t* const vga = (uint16_t* const) 0xB8000;
const uint16_t defaultColor = (COLOR8_LIGHT_GREY << 8) | (COLOR8_BLACK << 12);
uint16_t currentColor = defaultColor;

void Reset() {
    line = 0;
    column = 0;
    currentColor = defaultColor;

    for (uint16_t y = 0; y < HEIGHT; y++) {
        for (uint16_t x = 0; x < WIDTH; x++) {
            vga[y * WIDTH + x] = ' ' | currentColor;
        }
    }
}

void newLine() {
    if (line < HEIGHT - 1) {
        line++;
        column = 0;
    } else {
        scrollUp();
        column = 0;
    }
}

void scrollUp() {
    for (uint16_t y = 0; y < HEIGHT; y++) {
        for (uint16_t x = 0; x < WIDTH; x++) {
            vga[(y - 1) * WIDTH + x] = vga[y * WIDTH + x];
        }
    }

    for (uint16_t x = 0; x < WIDTH; x++) {
        vga[(HEIGHT - 1) * WIDTH + x] = ' ' | currentColor;
    }   
}

void print(const char* s) {
    while (*s) {
        switch (*s) {
            case '\n': {
                newLine();
                break;
            }

            case '\r': {
                column = 0;
                break;
            }

            case '\t': {
                if (column == WIDTH) {
                    newLine();
                }
                uint16_t tabLen = 4 - (column % 4);
                while (tabLen != 0) {
                    vga[line * WIDTH + (column++)] = ' ' | currentColor;
                    tabLen--;
                }
                break;
            }

            default: {
                if (column == WIDTH) {
                    newLine();
                }
                vga[line * WIDTH + (column++)] = *s | currentColor;
                break;
            }
        }

        s++;
    }
}

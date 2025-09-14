#include "vga.h"

void kernel(void) {
    Reset();
    print("Hello, World!\r\n");
}

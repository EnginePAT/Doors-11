#include "vga.h"
#include "stdint.h"
#include "gdt/gdt.h"
#include "interrupts/idt.h"
#include "timer.h"
#include "kmalloc.h"
#include "stdlib/stdio.h"
#include "keyboard.h"
#include "multiboot.h"
#include "memory.h"
#include "util.h"
#include "fs/fs.h"
#include "string.h"


void kernel_main(uint32_t magic, struct multiboot_info *bootInfo) {
    Reset();

    if (magic != 0x2BADB002) {
        print("Invalid GRUB magic!\n");
        for (;;);                           // Halt the system
    } else {
        print("Valid magic passed by GRUB!\n");
    }

    // --- GDT / IDT / Timer initialization ---
    if (!initGdt()) print("[ STATUS ERR ] Could not initialize the GDT!\r\n");
    else print("[ STATUS OK ] GDT initialized!\r\n");

    if (!initIdt()) print("[ STATUS ERR ] Could not initialize the IDT!\r\n");
    else print("[ STATUS OK ] IDT initialized!\r\n");

    if (!initTimer()) print("[ STATUS ERR ] Could not initialize the Timer!\r\n");
    else print("[ STATUS OK ] Timer initialized!\r\n");

    // --- Memory initialization ---
    // uint32_t memUpper = bootInfo->mem_upper * 1024; // size in bytes
    // uint32_t physicalAllocStart = 0x100000;         // start kernel heap at 1 MiB
    // if (!initMemory(memUpper, physicalAllocStart)) {
    //     print("Memory could not be initialized\n");
    // } else {
    //     print("[ STATUS OK ] Memory Successfully Initialized\n");
    // }
    
    // if (!kmallocInit(0x1000)) {
    //     print("kmalloc could not be initialized.\n");
    // }

    if (bootInfo->mods_count > 0) {
        multiboot_module_t* mods = (multiboot_module_t*) bootInfo->mods_addr;
        uint32_t mod_start = mods[0].mod_start;
        uint32_t mod_end   = mods[0].mod_end;
        uint32_t mod_size  = mod_end - mod_start;

        // Treat it as a pointer
        uint8_t* initrd = (uint8_t*) mod_start;

        // Example: dump first 16 bytes
        for (int i = 0; i < 16; i++) {
            printf("%x ", initrd[i]);
        }
        print("\n");
        fs_init((uint8_t*) mod_start);
    }

    // --- Keyboard ---
    initKeyboard();

    print("-----------------------\n\n");
    print("Doors OS Shell\n");
    print(">");

    for(;;); // infinite loop
}

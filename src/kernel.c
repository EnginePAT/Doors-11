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
#include "disk/fs/fs.h"
#include "string.h"
#include "disk/disk.h"


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

    uint8_t buffer[512];
    disk_init();
    disk_read_sector(0, buffer); // read MBR
    for(int i=0; i<16; i++) {
        print_hex8(buffer[i]);
    }

    // --- Keyboard ---
    initKeyboard();

    print("-----------------------\n\n");
    print("Doors OS Shell\n");
    print(">");

    for(;;); // infinite loop
}

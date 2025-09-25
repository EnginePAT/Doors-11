#include "vga.h"
#include "stdint.h"
#include "memory/gdt/gdt.h"
#include "memory/interrupts/idt.h"
#include "timer.h"
#include "memory/kmalloc.h"
#include "stdlib/stdio.h"
#include "keyboard.h"
#include "multiboot.h"
#include "memory/memory.h"
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
    //
    // if (!kmallocInit(0x1000)) {
    //     print("kmalloc could not be initialized.\n");
    // }

    // Check if GRUB provided modules
    // Make sure multiboot.h is correct (no extra fields in multiboot_info)

    if (bootInfo->flags & MULTIBOOT_FLAG_MODS) {  // check if modules exist
        if (bootInfo->mods_count > 0) {
            multiboot_module_t* module = (multiboot_module_t*)bootInfo->mods_addr;

            // Validate module addresses
            if (module->mod_start >= module->mod_end) {
                print("Invalid module addresses!\n");
            } else {
                // Initialize disk from module
                disk_init(module);

                print("Module start: ");
                print_hex32(module->mod_start);
                print("\nModule end: ");
                print_hex32(module->mod_end);
                print("\nModule size: ");
                print_hex32(module->mod_end - module->mod_start);
                print("\n");

                // Dump first 512 bytes
                uint8_t buffer[512];
                disk_read_bytes(0, buffer, 512);

                print("Dumping first 512 bytes of the module:\n");
                for (int i = 0; i < 16; i++) {
                    print_hex8(buffer[i]);
                    print(" ");
                    if ((i + 1) % 16 == 0) print("\n");
                }

                disk.data = (uint8_t*)(uintptr_t)module->mod_start;

                // Initialize filesystem using the disk module
                fs_init(disk.data);  // <- use the pointer from your disk driver
            }
        }
    } else {
        print("No GRUB modules loaded.\n");
    }

    // --- Keyboard ---
    if (!initKeyboard()) {
        print("[ STATUS ERR ] Could not initialize keyboard!\n");
    } else {
        print("[ STATUS OK ] Keyboard successfully initialized!\n");
    }

    print("-----------------------\n\n");
    print("Doors OS Shell\n");
    print("C:\\Users\\doors>");

    for(;;); // infinite loop
}

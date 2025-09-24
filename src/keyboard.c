#include "stdint.h"
#include "util.h"
#include "interrupts/idt.h"
#include "stdlib/stdio.h"
#include "keyboard.h"
#include "vga.h"
#include "string.h"

#define INPUT_BUFFER_SIZE 128

bool capsOn;
bool capsLock;

const uint32_t UNKNOWN = 0xFFFFFFFF;
const uint32_t ESC = 0xFFFFFFFF - 1;
const uint32_t CTRL = 0xFFFFFFFF - 2;
const uint32_t LSHFT = 0xFFFFFFFF - 3;
const uint32_t RSHFT = 0xFFFFFFFF - 4;
const uint32_t ALT = 0xFFFFFFFF - 5;
const uint32_t F1 = 0xFFFFFFFF - 6;
const uint32_t F2 = 0xFFFFFFFF - 7;
const uint32_t F3 = 0xFFFFFFFF - 8;
const uint32_t F4 = 0xFFFFFFFF - 9;
const uint32_t F5 = 0xFFFFFFFF - 10;
const uint32_t F6 = 0xFFFFFFFF - 11;
const uint32_t F7 = 0xFFFFFFFF - 12;
const uint32_t F8 = 0xFFFFFFFF - 13;
const uint32_t F9 = 0xFFFFFFFF - 14;
const uint32_t F10 = 0xFFFFFFFF - 15;
const uint32_t F11 = 0xFFFFFFFF - 16;
const uint32_t F12 = 0xFFFFFFFF - 17;
const uint32_t SCRLCK = 0xFFFFFFFF - 18;
const uint32_t HOME = 0xFFFFFFFF - 19;
const uint32_t UP = 0xFFFFFFFF - 20;
const uint32_t LEFT = 0xFFFFFFFF - 21;
const uint32_t RIGHT = 0xFFFFFFFF - 22;
const uint32_t DOWN = 0xFFFFFFFF - 23;
const uint32_t PGUP = 0xFFFFFFFF - 24;
const uint32_t PGDOWN = 0xFFFFFFFF - 25;
const uint32_t END = 0xFFFFFFFF - 26;
const uint32_t INS = 0xFFFFFFFF - 27;
const uint32_t DEL = 0xFFFFFFFF - 28;
const uint32_t CAPS = 0xFFFFFFFF - 29;
const uint32_t NONE = 0xFFFFFFFF - 30;
const uint32_t ALTGR = 0xFFFFFFFF - 31;
const uint32_t NUMLCK = 0xFFFFFFFF - 32;


const uint32_t lowercase[128] = {
UNKNOWN,ESC,'1','2','3','4','5','6','7','8',
'9','0','-','=','\b','\t','q','w','e','r',
't','y','u','i','o','p','[',']','\n',CTRL,
'a','s','d','f','g','h','j','k','l',';',
'\'','`',LSHFT,'\\','z','x','c','v','b','n','m',',',
'.','/',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',LEFT,UNKNOWN,RIGHT,
'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};

const uint32_t uppercase[128] = {
    UNKNOWN,ESC,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t','Q','W','E','R',
'T','Y','U','I','O','P','{','}','\n',CTRL,'A','S','D','F','G','H','J','K','L',':','"','~',LSHFT,'|','Z','X','C',
'V','B','N','M','<','>','?',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',
LEFT,UNKNOWN,RIGHT,'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};

// --- Shell input buffer ---
static char inputBuffer[INPUT_BUFFER_SIZE];
static int inputPos = 0;

#define PM1a_CNT_BLK 0x400  // Example port (real address depends on ACPI)
#define SLP_TYP 0x1C00      // Sleep type S5
#define SLP_EN  0x2000      // Sleep enable

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

void shutdown() {
    // Disable interrupts
    __asm__ volatile ("cli");

    // Send the S5 sleep command
    outw(PM1a_CNT_BLK, SLP_TYP | SLP_EN);

    // Halt CPU if shutdown fails
    for (;;) { __asm__ volatile("hlt"); }
}

void reboot() {
    // Wait until the keyboard controller input buffer is empty
    while ((inb(0x64) & 0x02) != 0);

    // Send the pulse reset command
    outb(0x64, 0xFE);

    // Halt in case it fails
    for (;;) { __asm__ __volatile__("hlt"); }
}

// --- Simple command runner ---
void runCommand(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        print("Commands: help, clear, echo\n");
    } else if (strcmp(cmd, "clear") == 0) {
        Reset(); // VGA clear
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        print(cmd + 5);
        print("\n");
    } else if(cmd[0] == '\0') {
        // do nothing
    } else if(strcmp(cmd, "reboot") == 0) {
        reboot();
    } else if(strcmp(cmd, "shutdown") == 0) {
        shutdown();
    } else {
        print("Unknown command: ");
        print(cmd);
        print("\n");
    }
}

// --- Buffer handler ---
void handleKey(char c) {
    if (c == '\n') {
        inputBuffer[inputPos] = 0;  // null-terminate
        print("\n");                // newline
        runCommand(inputBuffer);    // execute
        inputPos = 0;               // reset
        print(">");                // prompt
    } else if (c == '\b') {
        if (inputPos > 0) {
            inputPos--;
            print("\b \b");         // erase last char visually
        }
    } else if (inputPos < INPUT_BUFFER_SIZE - 1) {
        inputBuffer[inputPos++] = c;
        char s[2] = {c, 0};
        print(s);                  // echo typed char
    }
}

// --- Keyboard handler ---
void keyboardHandler(struct InterruptRegisters *regs) {
    uint8_t scancode = inb(0x60);
    bool released = scancode & 0x80;   // top bit = release
    scancode &= 0x7F;                  // remove top bit

    char c = 0;

    if (!released) {
        // Map scancode to ASCII
        if (capsOn || capsLock) {
            if (uppercase[scancode] <= 127)
                c = (char)uppercase[scancode];
        } else {
            if (lowercase[scancode] <= 127)
                c = (char)lowercase[scancode];
        }

        if (c)
            handleKey(c);

        // Handle special keys on key press
        switch(scancode) {
            case 42: // LShift down
            case 54: // RShift down
                capsOn = true;
                break;
            case 58: // CapsLock
                capsLock = !capsLock;
                break;
        }
    } else {
        // Handle key releases
        switch(scancode) {
            case 42: // LShift up
            case 54: // RShift up
                capsOn = false;
                break;
        }
    }
}

// --- Keyboard init ---
bool initKeyboard() {
    capsOn = false;
    capsLock = false;
    irq_install_handler(1, &keyboardHandler);
}

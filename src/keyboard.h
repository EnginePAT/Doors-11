#ifndef KEYBOARD_H
#define KEYBOARD_H

bool initKeyboard();
void keyboardHandler(struct InterruptRegisters *regs);

#endif      // KEYBOARD_H

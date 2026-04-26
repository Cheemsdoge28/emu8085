#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include "cpu.h"

// Executes a single opcode
void execute(CPU *cpu, uint8_t opcode);

#endif
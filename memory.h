#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

// 64KB memory (8085 address space)
extern uint8_t memory[65536];

// basic operations
uint8_t mem_read(uint16_t addr);
void mem_write(uint16_t addr, uint8_t value);

// load program into memory
void load_program(uint8_t *program, int size);

#endif
#include "memory.h"
#include <stdio.h>

uint8_t memory[65536];

uint8_t mem_read(uint16_t addr)
{
    return memory[addr];
}
void mem_write(uint16_t addr, uint8_t value)
{
    memory[addr] = value;
}

// load program into memory
void load_program(uint8_t *program, int size)
{
    for (int i = 0 ; i < size; i++)
    {
        memory[i] = program[i];
    }
}
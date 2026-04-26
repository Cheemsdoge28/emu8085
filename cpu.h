#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t A;        // Accumulator
    uint16_t PC;      // Program Counter
    uint8_t flags;    // Flags register
    uint16_t SP;      // Stack Pointer
    uint8_t B, C, D, E, H, L;
} CPU;

uint8_t fetch(CPU *cpu);
uint16_t fetch_16(CPU *cpu);
void log_cpu(CPU *cpu);

uint16_t get_HL(CPU *cpu);
void set_HL(CPU *cpu, uint16_t value);

uint16_t get_DE(CPU *cpu);
void set_DE(CPU *cpu, uint16_t value);

uint16_t get_BC(CPU *cpu);
void set_BC(CPU *cpu, uint16_t value);

// 8085 flags register bit positions
#define FLAG_CY 0x01
#define FLAG_P  0x04
#define FLAG_AC 0x10
#define FLAG_Z  0x40
#define FLAG_S  0x80

uint8_t get_carry_flag(CPU *cpu);
void set_carry_flag(CPU *cpu, uint8_t value);

uint8_t get_parity_flag(CPU *cpu);
void set_parity_flag(CPU *cpu, uint8_t value);

uint8_t get_aux_carry_flag(CPU *cpu);
void set_aux_carry_flag(CPU *cpu, uint8_t value);

uint8_t get_zero_flag(CPU *cpu);
void set_zero_flag(CPU *cpu, uint8_t value);

uint8_t get_sign_flag(CPU *cpu);
void set_sign_flag(CPU *cpu, uint8_t value);

void set_ZSP(CPU *cpu, uint8_t result);

void set_flags_add(CPU *cpu, uint8_t a, uint8_t b, uint8_t carry);

void set_flags_sub(CPU *cpu, uint8_t a, uint8_t b, uint8_t borrow);

#endif
#include "cpu.h"
#include "memory.h"

static uint8_t get_flag(CPU *cpu, uint8_t mask) {
    return (cpu->flags & mask) ? 1 : 0;
}

static void set_flag(CPU *cpu, uint8_t mask, uint8_t value) {
    if (value) {
        cpu->flags |= mask;
    } else {
        cpu->flags &= (uint8_t)(~mask);
    }
}

uint8_t fetch(CPU *cpu) {
    return mem_read(cpu->PC++);
}

uint16_t fetch_16(CPU *cpu) {
    return (mem_read(cpu->PC++) | (mem_read(cpu->PC++) << 8));
}

void log_cpu(CPU *cpu) {
    printf(
        "PC=%04X | A=%02X B=%02X C=%02X D=%02X E=%02X H=%02X L=%02X | FLAGS=%02X\n",
        cpu->PC,
        cpu->A, cpu->B, cpu->C,
        cpu->D, cpu->E,
        cpu->H, cpu->L,
        cpu->flags
    );
}

uint16_t get_HL(CPU *cpu) {
    return ((uint16_t)cpu->H << 8) | cpu->L;
}

void set_HL(CPU *cpu, uint16_t value) {
    cpu->H = (value >> 8) & 0xFF;
    cpu->L = value & 0xFF;
}

uint16_t get_DE(CPU *cpu) {
    return ((uint16_t)cpu->D << 8) | cpu->E;
}

void set_DE(CPU *cpu, uint16_t value) {
    cpu->D = (value >> 8) & 0xFF;
    cpu->E = value & 0xFF;
}

uint16_t get_BC(CPU *cpu) {
    return ((uint16_t)cpu->B << 8) | cpu->C;
}

void set_BC(CPU *cpu, uint16_t value) {
    cpu->B = (value >> 8) & 0xFF;
    cpu->C = value & 0xFF;
}

uint8_t get_carry_flag(CPU *cpu) {
    return get_flag(cpu, FLAG_CY);
}

void set_carry_flag(CPU *cpu, uint8_t value) {
    set_flag(cpu, FLAG_CY, value);
}

uint8_t get_parity_flag(CPU *cpu) {
    return get_flag(cpu, FLAG_P);
}

void set_parity_flag(CPU *cpu, uint8_t value) {
    set_flag(cpu, FLAG_P, value);
}

uint8_t get_aux_carry_flag(CPU *cpu) {
    return get_flag(cpu, FLAG_AC);
}

void set_aux_carry_flag(CPU *cpu, uint8_t value) {
    set_flag(cpu, FLAG_AC, value);
}

uint8_t get_zero_flag(CPU *cpu) {
    return get_flag(cpu, FLAG_Z);
}

void set_zero_flag(CPU *cpu, uint8_t value) {
    set_flag(cpu, FLAG_Z, value);
}

uint8_t get_sign_flag(CPU *cpu) {
    return get_flag(cpu, FLAG_S);
}

void set_ZSP(CPU *cpu, uint8_t result) {
    // Zero
    if (result == 0) cpu->flags |= FLAG_Z;
    else cpu->flags &= ~FLAG_Z;

    // Sign
    if (result & 0x80) cpu->flags |= FLAG_S;
    else cpu->flags &= ~FLAG_S;

    // Parity
    int count = 0;
    for (int i = 0; i < 8; i++)
        if (result & (1 << i)) count++;

    if (count % 2 == 0) cpu->flags |= FLAG_P;
    else cpu->flags &= ~FLAG_P;
};

void set_flags_add(CPU *cpu, uint8_t a, uint8_t b, uint8_t carry) {
    uint16_t result = a + b + carry;

    // CY
    if (result > 0xFF) cpu->flags |= FLAG_CY;
    else cpu->flags &= ~FLAG_CY;

    // AC
    if (((a & 0x0F) + (b & 0x0F) + carry) > 0x0F)
        cpu->flags |= FLAG_AC;
    else
        cpu->flags &= ~FLAG_AC;

    set_ZSP(cpu, result & 0xFF);
};

void set_flags_sub(CPU *cpu, uint8_t a, uint8_t b, uint8_t borrow) {
    uint16_t result = a - b - borrow;

    // CY (borrow)
    if (a < (b + borrow)) cpu->flags |= FLAG_CY;
    else cpu->flags &= ~FLAG_CY;

    // AC
    if ((a & 0x0F) < ((b & 0x0F) + borrow))
        cpu->flags |= FLAG_AC;
    else
        cpu->flags &= ~FLAG_AC;

    set_ZSP(cpu, result & 0xFF);
}


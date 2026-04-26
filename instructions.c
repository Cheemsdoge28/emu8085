#include <stdio.h>
#include "instructions.h"
#include "memory.h" // needed for fetch via CPU
#include <stdlib.h> // needed for exit()

void execute(CPU *cpu, uint8_t opcode)
{
    switch (opcode)
    {
    // DATA TRANSFER INSTRUCTIONS
    // MVI r, data
    case 0x3E: // MVI A, data
        cpu->A = fetch(cpu);
        break;
    case 0x06: // MVI B, data
        cpu->B = fetch(cpu);
        break;
    case 0x0E: // MVI C, data
        cpu->C = fetch(cpu);
        break;
    case 0x16: // MVI D, data
        cpu->D = fetch(cpu);
        break;
    case 0x1E: // MVI E, data
        cpu->E = fetch(cpu);
        break;
    case 0x26: // MVI H, data
        cpu->H = fetch(cpu);
        break;
    case 0x2E: // MVI L, data
        cpu->L = fetch(cpu);
        break;
    // STAX
    case 0x02: // STAX B
        mem_write(get_BC(cpu), cpu->A);
        break;
    case 0x12: // STAX D
        mem_write(get_DE(cpu), cpu->A);
        break;
    // LDAX
    case 0x0A: // LDAX B
        cpu->A = mem_read(get_BC(cpu));
        break;
    case 0x1A: // LDAX D
        cpu->A = mem_read(get_DE(cpu));
        break;
    // Exchange
    case 0xEB: // XCHG
    {
        uint16_t hl = get_HL(cpu);
        uint16_t de = get_DE(cpu);
        set_HL(cpu, de);
        set_DE(cpu, hl);
        break;
    }
    case 0xE9: // PCHL
    {
        cpu->PC = get_HL(cpu);
        break;
    }
    // ARITHMETIC & LOGIC
    case 0xC6:
    { // ADI
        uint8_t value = fetch(cpu);
        uint8_t carry = 0;
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x87:
    { // ADD A
        uint8_t value = cpu->A;
        uint8_t carry = 0;
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x80:
    { // ADD B
        uint8_t value = cpu->B;
        uint8_t carry = 0;
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x81:
    { // ADD C
        uint8_t value = cpu->C;
        uint8_t carry = 0;
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x82:
    { // ADD D
        uint8_t value = cpu->D;
        uint8_t carry = 0;
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x83:
    { // ADD E
        uint8_t value = cpu->E;
        uint8_t carry = 0;
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x84:
    { // ADD H
        uint8_t value = cpu->H;
        uint8_t carry = 0;
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x85:
    { // ADD L
        uint8_t value = cpu->L;
        uint8_t carry = 0;
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x86:
    { // ADD M
        uint8_t value = mem_read(get_HL(cpu));
        uint8_t carry = 0;
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0xCE:
    { // ACI
        uint8_t value = fetch(cpu);
        uint8_t carry = get_carry_flag(cpu);
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x8F:
    { // ADC A
        uint8_t value = cpu->A;
        uint8_t carry = get_carry_flag(cpu);
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x88:
    { // ADC B
        uint8_t value = cpu->B;
        uint8_t carry = get_carry_flag(cpu);
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x89:
    { // ADC C
        uint8_t value = cpu->C;
        uint8_t carry = get_carry_flag(cpu);
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x8A:
    { // ADC D
        uint8_t value = cpu->D;
        uint8_t carry = get_carry_flag(cpu);
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x8B:
    { // ADC E
        uint8_t value = cpu->E;
        uint8_t carry = get_carry_flag(cpu);
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x8C:
    { // ADC H
        uint8_t value = cpu->H;
        uint8_t carry = get_carry_flag(cpu);
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x8D:
    { // ADC L
        uint8_t value = cpu->L;
        uint8_t carry = get_carry_flag(cpu);
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0x8E:
    { // ADC M
        uint8_t value = mem_read(get_HL(cpu));
        uint8_t carry = get_carry_flag(cpu);
        uint16_t result = cpu->A + value + carry;

        set_flags_add(cpu, cpu->A, value, carry);
        cpu->A = result & 0xFF;

        break;
    }
    case 0xA6:
    { // ANA M
        uint8_t value = mem_read(get_HL(cpu));
        uint8_t result = cpu->A & value;

        cpu->A = result;

        // Z, S, P
        set_ZSP(cpu, result);

        // CY = 0
        cpu->flags &= ~FLAG_CY;

        // AC = 1
        cpu->flags |= FLAG_AC;

        break;
    }
    case 0xA0:
    { // ANA B
        uint8_t result = cpu->A & cpu->B;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags |= FLAG_AC;

        break;
    }
    case 0xA1:
    { // ANA C
        uint8_t result = cpu->A & cpu->C;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags |= FLAG_AC;

        break;
    }
    case 0xA2:
    { // ANA D
        uint8_t result = cpu->A & cpu->D;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags |= FLAG_AC;

        break;
    }
    case 0xA3:
    { // ANA E
        uint8_t result = cpu->A & cpu->E;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags |= FLAG_AC;

        break;
    }
    case 0xA4:
    { // ANA H
        uint8_t result = cpu->A & cpu->H;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags |= FLAG_AC;

        break;
    }
    case 0xA5:
    { // ANA L
        uint8_t result = cpu->A & cpu->L;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags |= FLAG_AC;

        break;
    }
    case 0xA7:
    { // ANA A
        uint8_t result = cpu->A & cpu->A;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags |= FLAG_AC;

        break;
    }
    case 0xE6:
    { // ANI data
        uint8_t value = fetch(cpu);
        uint8_t result = cpu->A & value;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags |= FLAG_AC;

        break;
    }
    

    // CONTROL INSTRUCTIONS
    case 0x76: // HLT
        printf("HLT encountered. Halting execution.\n");
        exit(0);
        break;
    default:
        printf("Unknown opcode: %02X\n", opcode);
        break;
    }
}
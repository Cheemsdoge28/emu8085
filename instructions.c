#include <stdio.h>
#include "instructions.h"
#include "memory.h" // needed for fetch via CPU
#include <stdlib.h> // needed for exit()

static void push16(CPU *cpu, uint16_t value)
{
    cpu->SP--;
    mem_write(cpu->SP, (uint8_t)((value >> 8) & 0xFF));
    cpu->SP--;
    mem_write(cpu->SP, (uint8_t)(value & 0xFF));
}

static uint16_t pop16(CPU *cpu)
{
    uint8_t low = mem_read(cpu->SP);
    cpu->SP++;
    uint8_t high = mem_read(cpu->SP);
    cpu->SP++;
    return ((uint16_t)high << 8) | low;
}

static uint8_t read_reg_code(CPU *cpu, uint8_t code)
{
    switch (code)
    {
    case 0: return cpu->B;
    case 1: return cpu->C;
    case 2: return cpu->D;
    case 3: return cpu->E;
    case 4: return cpu->H;
    case 5: return cpu->L;
    case 6: return mem_read(get_HL(cpu));
    default: return cpu->A;
    }
}

static void write_reg_code(CPU *cpu, uint8_t code, uint8_t value)
{
    switch (code)
    {
    case 0: cpu->B = value; break;
    case 1: cpu->C = value; break;
    case 2: cpu->D = value; break;
    case 3: cpu->E = value; break;
    case 4: cpu->H = value; break;
    case 5: cpu->L = value; break;
    case 6: mem_write(get_HL(cpu), value); break;
    default: cpu->A = value; break;
    }
}

static uint8_t inr8(CPU *cpu, uint8_t value)
{
    uint8_t old_cy = get_carry_flag(cpu);
    uint8_t result = (uint8_t)(value + 1);
    set_flags_add(cpu, value, 1, 0);
    set_carry_flag(cpu, old_cy);
    return result;
}

static uint8_t dcr8(CPU *cpu, uint8_t value)
{
    uint8_t old_cy = get_carry_flag(cpu);
    uint8_t result = (uint8_t)(value - 1);
    set_flags_sub(cpu, value, 1, 0);
    set_carry_flag(cpu, old_cy);
    return result;
}

void execute(CPU *cpu, uint8_t opcode)
{
    // MOV r1,r2 block (0x40-0x7F except HLT at 0x76)
    if ((opcode & 0xC0) == 0x40 && opcode != 0x76)
    {
        uint8_t dest = (opcode >> 3) & 0x07;
        uint8_t src = opcode & 0x07;
        write_reg_code(cpu, dest, read_reg_code(cpu, src));
        return;
    }

    switch (opcode)
    {
    // DATA TRANSFER INSTRUCTIONS
    case 0x00: // NOP
        break;
    case 0x08: // Undocumented
    case 0x10: // Undocumented
    case 0x18: // Undocumented
    case 0x28: // Undocumented
    case 0x38: // Undocumented
    case 0xCB: // Undocumented
    case 0xDD: // Undocumented
    case 0xED: // Undocumented
    case 0xFD: // Undocumented
        break;
    case 0x01: // LXI B, d16
        set_BC(cpu, fetch_16(cpu));
        break;
    case 0x11: // LXI D, d16
        set_DE(cpu, fetch_16(cpu));
        break;
    case 0x21: // LXI H, d16
        set_HL(cpu, fetch_16(cpu));
        break;
    case 0x31: // LXI SP, d16
        cpu->SP = fetch_16(cpu);
        break;
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
    case 0x36: // MVI M, data
        mem_write(get_HL(cpu), fetch(cpu));
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
    case 0x3A: // LDA addr
        cpu->A = mem_read(fetch_16(cpu));
        break;
    case 0x32: // STA addr
        mem_write(fetch_16(cpu), cpu->A);
        break;
    case 0x2A: // LHLD addr
    {
        uint16_t addr = fetch_16(cpu);
        cpu->L = mem_read(addr);
        cpu->H = mem_read(addr + 1);
        break;
    }
    case 0x22: // SHLD addr
    {
        uint16_t addr = fetch_16(cpu);
        mem_write(addr, cpu->L);
        mem_write(addr + 1, cpu->H);
        break;
    }
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
    case 0xE3: // XTHL
    {
        uint8_t low = mem_read(cpu->SP);
        uint8_t high = mem_read(cpu->SP + 1);
        mem_write(cpu->SP, cpu->L);
        mem_write(cpu->SP + 1, cpu->H);
        cpu->L = low;
        cpu->H = high;
        break;
    }
    case 0xF9: // SPHL
        cpu->SP = get_HL(cpu);
        break;
    case 0x07: // RLC
    {
        uint8_t msb = (cpu->A >> 7) & 0x01;
        cpu->A = (uint8_t)((cpu->A << 1) | msb);
        set_carry_flag(cpu, msb);
        break;
    }
    case 0x0F: // RRC
    {
        uint8_t lsb = cpu->A & 0x01;
        cpu->A = (uint8_t)((cpu->A >> 1) | (lsb << 7));
        set_carry_flag(cpu, lsb);
        break;
    }
    case 0x17: // RAL
    {
        uint8_t old_cy = get_carry_flag(cpu);
        uint8_t msb = (cpu->A >> 7) & 0x01;
        cpu->A = (uint8_t)((cpu->A << 1) | old_cy);
        set_carry_flag(cpu, msb);
        break;
    }
    case 0x1F: // RAR
    {
        uint8_t old_cy = get_carry_flag(cpu);
        uint8_t lsb = cpu->A & 0x01;
        cpu->A = (uint8_t)((cpu->A >> 1) | (old_cy << 7));
        set_carry_flag(cpu, lsb);
        break;
    }
    case 0x2F: // CMA
        cpu->A = (uint8_t)~cpu->A;
        break;
    case 0x3F: // CMC
        set_carry_flag(cpu, !get_carry_flag(cpu));
        break;
    case 0x37: // STC
        set_carry_flag(cpu, 1);
        break;
    case 0x27: // DAA
    {
        uint8_t old_a = cpu->A;
        uint8_t old_cy = get_carry_flag(cpu);
        uint8_t correction = 0;

        if (((old_a & 0x0F) > 9) || get_aux_carry_flag(cpu))
            correction |= 0x06;

        if ((old_a > 0x99) || old_cy)
            correction |= 0x60;

        set_flags_add(cpu, old_a, correction, 0);
        cpu->A = (uint8_t)(old_a + correction);
        set_carry_flag(cpu, old_cy || get_carry_flag(cpu));
        break;
    }

    // 16-bit INX/DCX/DAD
    case 0x03: // INX B
        set_BC(cpu, get_BC(cpu) + 1);
        break;
    case 0x13: // INX D
        set_DE(cpu, get_DE(cpu) + 1);
        break;
    case 0x23: // INX H
        set_HL(cpu, get_HL(cpu) + 1);
        break;
    case 0x33: // INX SP
        cpu->SP++;
        break;
    case 0x0B: // DCX B
        set_BC(cpu, get_BC(cpu) - 1);
        break;
    case 0x1B: // DCX D
        set_DE(cpu, get_DE(cpu) - 1);
        break;
    case 0x2B: // DCX H
        set_HL(cpu, get_HL(cpu) - 1);
        break;
    case 0x3B: // DCX SP
        cpu->SP--;
        break;
    case 0x09: // DAD B
    {
        uint32_t result = (uint32_t)get_HL(cpu) + get_BC(cpu);
        set_HL(cpu, (uint16_t)(result & 0xFFFF));
        set_carry_flag(cpu, result > 0xFFFF);
        break;
    }
    case 0x19: // DAD D
    {
        uint32_t result = (uint32_t)get_HL(cpu) + get_DE(cpu);
        set_HL(cpu, (uint16_t)(result & 0xFFFF));
        set_carry_flag(cpu, result > 0xFFFF);
        break;
    }
    case 0x29: // DAD H
    {
        uint32_t result = (uint32_t)get_HL(cpu) + get_HL(cpu);
        set_HL(cpu, (uint16_t)(result & 0xFFFF));
        set_carry_flag(cpu, result > 0xFFFF);
        break;
    }
    case 0x39: // DAD SP
    {
        uint32_t result = (uint32_t)get_HL(cpu) + cpu->SP;
        set_HL(cpu, (uint16_t)(result & 0xFFFF));
        set_carry_flag(cpu, result > 0xFFFF);
        break;
    }

    // INR/DCR
    case 0x04: cpu->B = inr8(cpu, cpu->B); break; // INR B
    case 0x0C: cpu->C = inr8(cpu, cpu->C); break; // INR C
    case 0x14: cpu->D = inr8(cpu, cpu->D); break; // INR D
    case 0x1C: cpu->E = inr8(cpu, cpu->E); break; // INR E
    case 0x24: cpu->H = inr8(cpu, cpu->H); break; // INR H
    case 0x2C: cpu->L = inr8(cpu, cpu->L); break; // INR L
    case 0x34: mem_write(get_HL(cpu), inr8(cpu, mem_read(get_HL(cpu)))); break; // INR M
    case 0x3C: cpu->A = inr8(cpu, cpu->A); break; // INR A
    case 0x05: cpu->B = dcr8(cpu, cpu->B); break; // DCR B
    case 0x0D: cpu->C = dcr8(cpu, cpu->C); break; // DCR C
    case 0x15: cpu->D = dcr8(cpu, cpu->D); break; // DCR D
    case 0x1D: cpu->E = dcr8(cpu, cpu->E); break; // DCR E
    case 0x25: cpu->H = dcr8(cpu, cpu->H); break; // DCR H
    case 0x2D: cpu->L = dcr8(cpu, cpu->L); break; // DCR L
    case 0x35: mem_write(get_HL(cpu), dcr8(cpu, mem_read(get_HL(cpu)))); break; // DCR M
    case 0x3D: cpu->A = dcr8(cpu, cpu->A); break; // DCR A

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
    case 0xAE:
    { // XRA M
        uint8_t value = mem_read(get_HL(cpu));
        uint8_t result = cpu->A ^ value;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xA8:
    { // XRA B
        uint8_t result = cpu->A ^ cpu->B;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xA9:
    { // XRA C
        uint8_t result = cpu->A ^ cpu->C;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xAA:
    { // XRA D
        uint8_t result = cpu->A ^ cpu->D;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xAB:
    { // XRA E
        uint8_t result = cpu->A ^ cpu->E;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xAC:
    { // XRA H
        uint8_t result = cpu->A ^ cpu->H;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xAD:
    { // XRA L
        uint8_t result = cpu->A ^ cpu->L;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xAF:
    { // XRA A
        uint8_t result = cpu->A ^ cpu->A;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xEE:
    { // XRI data
        uint8_t value = fetch(cpu);
        uint8_t result = cpu->A ^ value;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xB6:
    { // ORA M
        uint8_t value = mem_read(get_HL(cpu));
        uint8_t result = cpu->A | value;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xB0:
    { // ORA B
        uint8_t result = cpu->A | cpu->B;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xB1:
    { // ORA C
        uint8_t result = cpu->A | cpu->C;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xB2:
    { // ORA D
        uint8_t result = cpu->A | cpu->D;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xB3:
    { // ORA E
        uint8_t result = cpu->A | cpu->E;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xB4:
    { // ORA H
        uint8_t result = cpu->A | cpu->H;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xB5:
    { // ORA L
        uint8_t result = cpu->A | cpu->L;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xB7:
    { // ORA A
        uint8_t result = cpu->A | cpu->A;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xF6:
    { // ORI data
        uint8_t value = fetch(cpu);
        uint8_t result = cpu->A | value;

        cpu->A = result;

        set_ZSP(cpu, result);

        cpu->flags &= ~FLAG_CY;
        cpu->flags &= ~FLAG_AC;

        break;
    }
    case 0xBE:
    { // CMP M
        uint8_t value = mem_read(get_HL(cpu));
        set_flags_sub(cpu, cpu->A, value, 0);
        break;
    }
    case 0xB8:
    { // CMP B
        set_flags_sub(cpu, cpu->A, cpu->B, 0);
        break;
    }
    case 0xB9:
    { // CMP C
        set_flags_sub(cpu, cpu->A, cpu->C, 0);
        break;
    }
    case 0xBA:
    { // CMP D
        set_flags_sub(cpu, cpu->A, cpu->D, 0);
        break;
    }
    case 0xBB:
    { // CMP E
        set_flags_sub(cpu, cpu->A, cpu->E, 0);
        break;
    }
    case 0xBC:
    { // CMP H
        set_flags_sub(cpu, cpu->A, cpu->H, 0);
        break;
    }
    case 0xBD:
    { // CMP L
        set_flags_sub(cpu, cpu->A, cpu->L, 0);
        break;
    }
    case 0xBF:
    { // CMP A
        set_flags_sub(cpu, cpu->A, cpu->A, 0);
        break;
    }
    case 0xFE:
    { // CPI data
        uint8_t value = fetch(cpu);
        set_flags_sub(cpu, cpu->A, value, 0);
        break;
    }
    case 0xD6:
    { // SUI data
        uint8_t value = fetch(cpu);
        uint16_t result = cpu->A - value;
        set_flags_sub(cpu, cpu->A, value, 0);
        cpu->A = (uint8_t)(result & 0xFF);
        break;
    }
    case 0xDE:
    { // SBI data
        uint8_t value = fetch(cpu);
        uint8_t borrow = get_carry_flag(cpu);
        uint16_t result = cpu->A - value - borrow;
        set_flags_sub(cpu, cpu->A, value, borrow);
        cpu->A = (uint8_t)(result & 0xFF);
        break;
    }
    case 0x97: // SUB A
        set_flags_sub(cpu, cpu->A, cpu->A, 0);
        cpu->A = cpu->A - cpu->A;
        break;
    case 0x90: // SUB B
        set_flags_sub(cpu, cpu->A, cpu->B, 0);
        cpu->A = cpu->A - cpu->B;
        break;
    case 0x91: // SUB C
        set_flags_sub(cpu, cpu->A, cpu->C, 0);
        cpu->A = cpu->A - cpu->C;
        break;
    case 0x92: // SUB D
        set_flags_sub(cpu, cpu->A, cpu->D, 0);
        cpu->A = cpu->A - cpu->D;
        break;
    case 0x93: // SUB E
        set_flags_sub(cpu, cpu->A, cpu->E, 0);
        cpu->A = cpu->A - cpu->E;
        break;
    case 0x94: // SUB H
        set_flags_sub(cpu, cpu->A, cpu->H, 0);
        cpu->A = cpu->A - cpu->H;
        break;
    case 0x95: // SUB L
        set_flags_sub(cpu, cpu->A, cpu->L, 0);
        cpu->A = cpu->A - cpu->L;
        break;
    case 0x96: // SUB M
    {
        uint8_t value = mem_read(get_HL(cpu));
        set_flags_sub(cpu, cpu->A, value, 0);
        cpu->A = cpu->A - value;
        break;
    }
    case 0x9F: // SBB A
    {
        uint8_t borrow = get_carry_flag(cpu);
        set_flags_sub(cpu, cpu->A, cpu->A, borrow);
        cpu->A = cpu->A - cpu->A - borrow;
        break;
    }
    case 0x98: // SBB B
    {
        uint8_t borrow = get_carry_flag(cpu);
        set_flags_sub(cpu, cpu->A, cpu->B, borrow);
        cpu->A = cpu->A - cpu->B - borrow;
        break;
    }
    case 0x99: // SBB C
    {
        uint8_t borrow = get_carry_flag(cpu);
        set_flags_sub(cpu, cpu->A, cpu->C, borrow);
        cpu->A = cpu->A - cpu->C - borrow;
        break;
    }
    case 0x9A: // SBB D
    {
        uint8_t borrow = get_carry_flag(cpu);
        set_flags_sub(cpu, cpu->A, cpu->D, borrow);
        cpu->A = cpu->A - cpu->D - borrow;
        break;
    }
    case 0x9B: // SBB E
    {
        uint8_t borrow = get_carry_flag(cpu);
        set_flags_sub(cpu, cpu->A, cpu->E, borrow);
        cpu->A = cpu->A - cpu->E - borrow;
        break;
    }
    case 0x9C: // SBB H
    {
        uint8_t borrow = get_carry_flag(cpu);
        set_flags_sub(cpu, cpu->A, cpu->H, borrow);
        cpu->A = cpu->A - cpu->H - borrow;
        break;
    }
    case 0x9D: // SBB L
    {
        uint8_t borrow = get_carry_flag(cpu);
        set_flags_sub(cpu, cpu->A, cpu->L, borrow);
        cpu->A = cpu->A - cpu->L - borrow;
        break;
    }
    case 0x9E: // SBB M
    {
        uint8_t value = mem_read(get_HL(cpu));
        uint8_t borrow = get_carry_flag(cpu);
        set_flags_sub(cpu, cpu->A, value, borrow);
        cpu->A = cpu->A - value - borrow;
        break;
    }

    // CONTROL INSTRUCTIONS
    case 0x76: // HLT
        printf("HLT encountered. Halting execution.\n");
        exit(0);
        break;
    case 0xC3: // JMP addr
        cpu->PC = fetch_16(cpu);
        break;
    case 0xC2: // JNZ addr
    {
        uint16_t addr = fetch_16(cpu);
        if (!get_zero_flag(cpu))
            cpu->PC = addr;
        break;
    }
    case 0xCA: // JZ addr
    {
        uint16_t addr = fetch_16(cpu);
        if (get_zero_flag(cpu))
            cpu->PC = addr;
        break;
    }
    case 0xD2: // JNC addr
    {
        uint16_t addr = fetch_16(cpu);
        if (!get_carry_flag(cpu))
            cpu->PC = addr;
        break;
    }
    case 0xDA: // JC addr
    {
        uint16_t addr = fetch_16(cpu);
        if (get_carry_flag(cpu))
            cpu->PC = addr;
        break;
    }
    case 0xE2: // JPO addr
    {
        uint16_t addr = fetch_16(cpu);
        if (!get_parity_flag(cpu))
            cpu->PC = addr;
        break;
    }
    case 0xEA: // JPE addr
    {
        uint16_t addr = fetch_16(cpu);
        if (get_parity_flag(cpu))
            cpu->PC = addr;
        break;
    }
    case 0xF2: // JP addr
    {
        uint16_t addr = fetch_16(cpu);
        if (!get_sign_flag(cpu))
            cpu->PC = addr;
        break;
    }
    case 0xFA: // JM addr
    {
        uint16_t addr = fetch_16(cpu);
        if (get_sign_flag(cpu))
            cpu->PC = addr;
        break;
    }
    case 0xC5: // PUSH B
        push16(cpu, get_BC(cpu));
        break;
    case 0xD5: // PUSH D
        push16(cpu, get_DE(cpu));
        break;
    case 0xE5: // PUSH H
        push16(cpu, get_HL(cpu));
        break;
    case 0xF5: // PUSH PSW
        push16(cpu, ((uint16_t)cpu->A << 8) | cpu->flags);
        break;
    case 0xC1: // POP B
        set_BC(cpu, pop16(cpu));
        break;
    case 0xD1: // POP D
        set_DE(cpu, pop16(cpu));
        break;
    case 0xE1: // POP H
        set_HL(cpu, pop16(cpu));
        break;
    case 0xF1: // POP PSW
    {
        uint16_t psw = pop16(cpu);
        cpu->A = (uint8_t)((psw >> 8) & 0xFF);
        cpu->flags = (uint8_t)(psw & 0xFF);
        break;
    }
    case 0xCD:
    { // CALL addr
        uint16_t addr = fetch_16(cpu);

        push16(cpu, cpu->PC);
        cpu->PC = addr;

        break;
    }
    case 0xC4: // CNZ addr
    {
        uint16_t addr = fetch_16(cpu);
        if (!get_zero_flag(cpu)) {
            push16(cpu, cpu->PC);
            cpu->PC = addr;
        }
        break;
    }
    case 0xCC: // CZ addr
    {
        uint16_t addr = fetch_16(cpu);
        if (get_zero_flag(cpu)) {
            push16(cpu, cpu->PC);
            cpu->PC = addr;
        }
        break;
    }
    case 0xD4: // CNC addr
    {
        uint16_t addr = fetch_16(cpu);
        if (!get_carry_flag(cpu)) {
            push16(cpu, cpu->PC);
            cpu->PC = addr;
        }
        break;
    }
    case 0xDC: // CC addr
    {
        uint16_t addr = fetch_16(cpu);
        if (get_carry_flag(cpu)) {
            push16(cpu, cpu->PC);
            cpu->PC = addr;
        }
        break;
    }
    case 0xE4: // CPO addr
    {
        uint16_t addr = fetch_16(cpu);
        if (!get_parity_flag(cpu)) {
            push16(cpu, cpu->PC);
            cpu->PC = addr;
        }
        break;
    }
    case 0xEC: // CPE addr
    {
        uint16_t addr = fetch_16(cpu);
        if (get_parity_flag(cpu)) {
            push16(cpu, cpu->PC);
            cpu->PC = addr;
        }
        break;
    }
    case 0xF4: // CP addr
    {
        uint16_t addr = fetch_16(cpu);
        if (!get_sign_flag(cpu)) {
            push16(cpu, cpu->PC);
            cpu->PC = addr;
        }
        break;
    }
    case 0xFC: // CM addr
    {
        uint16_t addr = fetch_16(cpu);
        if (get_sign_flag(cpu)) {
            push16(cpu, cpu->PC);
            cpu->PC = addr;
        }
        break;
    }
    case 0xC9:
    { // RET
        cpu->PC = pop16(cpu);
        break;
    }
    case 0xD9: // RETI
        cpu->PC = pop16(cpu);
        break;
    case 0xC7: // RST 0
        push16(cpu, cpu->PC);
        cpu->PC = 0x00;
        break;
    case 0xCF: // RST 1
        push16(cpu, cpu->PC);
        cpu->PC = 0x08;
        break;
    case 0xD7: // RST 2
        push16(cpu, cpu->PC);
        cpu->PC = 0x10;
        break;
    case 0xDF: // RST 3
        push16(cpu, cpu->PC);
        cpu->PC = 0x18;
        break;
    case 0xE7: // RST 4
        push16(cpu, cpu->PC);
        cpu->PC = 0x20;
        break;
    case 0xEF: // RST 5
        push16(cpu, cpu->PC);
        cpu->PC = 0x28;
        break;
    case 0xF7: // RST 6
        push16(cpu, cpu->PC);
        cpu->PC = 0x30;
        break;
    case 0xFF: // RST 7
        push16(cpu, cpu->PC);
        cpu->PC = 0x38;
        break;
    case 0xD3: // OUT port
    {
        uint8_t port = fetch(cpu);
        cpu->ports[port] = cpu->A;
        break;
    }
    case 0xDB: // IN port
    {
        uint8_t port = fetch(cpu);
        cpu->A = cpu->ports[port];
        break;
    }
    case 0xF3: // DI
        cpu->interrupt_enabled = 0;
        break;
    case 0xFB: // EI
        cpu->interrupt_enabled = 1;
        break;
    case 0x30: // SIM
    {
        uint8_t acc = cpu->A;

        // MSE bit enables writing interrupt masks from A[2:0].
        if (acc & 0x08) {
            cpu->interrupt_mask_5_5 = acc & 0x01;
            cpu->interrupt_mask_6_5 = (acc >> 1) & 0x01;
            cpu->interrupt_mask_7_5 = (acc >> 2) & 0x01;
        }

        // Reset RST 7.5 pending latch.
        if (acc & 0x10)
            cpu->interrupt_pending_7_5 = 0;

        // SDE enables writing serial output data from SOD bit.
        if (acc & 0x40)
            cpu->serial_output_data = (acc >> 7) & 0x01;

        break;
    }
    case 0x20: // RIM
        cpu->A =
            (uint8_t)(cpu->interrupt_mask_5_5 & 0x01) |
            (uint8_t)((cpu->interrupt_mask_6_5 & 0x01) << 1) |
            (uint8_t)((cpu->interrupt_mask_7_5 & 0x01) << 2) |
            (uint8_t)((cpu->interrupt_enabled & 0x01) << 3) |
            (uint8_t)((cpu->interrupt_pending_5_5 & 0x01) << 4) |
            (uint8_t)((cpu->interrupt_pending_6_5 & 0x01) << 5) |
            (uint8_t)((cpu->interrupt_pending_7_5 & 0x01) << 6) |
            (uint8_t)((cpu->serial_input_data & 0x01) << 7);
        break;
    case 0xC0: // RNZ
        if (!get_zero_flag(cpu))
            cpu->PC = pop16(cpu);
        break;
    case 0xC8: // RZ
        if (get_zero_flag(cpu))
            cpu->PC = pop16(cpu);
        break;
    case 0xD0: // RNC
        if (!get_carry_flag(cpu))
            cpu->PC = pop16(cpu);
        break;
    case 0xD8: // RC
        if (get_carry_flag(cpu))
            cpu->PC = pop16(cpu);
        break;
    case 0xE0: // RPO
        if (!get_parity_flag(cpu))
            cpu->PC = pop16(cpu);
        break;
    case 0xE8: // RPE
        if (get_parity_flag(cpu))
            cpu->PC = pop16(cpu);
        break;
    case 0xF0: // RP
        if (!get_sign_flag(cpu))
            cpu->PC = pop16(cpu);
        break;
    case 0xF8: // RM
        if (get_sign_flag(cpu))
            cpu->PC = pop16(cpu);
        break;
    default:
        printf("Unknown opcode: %02X at PC=%04X\n", opcode, cpu->PC - 1);
        break;
    }
}
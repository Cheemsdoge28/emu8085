#include "cpu.h"
#include "memory.h"
#include "instructions.h"

int main(void)
{
    CPU cpu = {0}; // Initialize CPU state
    cpu.SP = 0xFFFF;
    memory[0x2000] = 1;
    memory[0x2001] = 2;
    memory[0x2002] = 3;
    memory[0x2003] = 4;
    memory[0x2004] = 5;
    uint8_t program[] = {
        0x21, 0x00, 0x20, // LXI H,2000   → HL = start of array
        0x06, 0x05,       // MVI B,05     → counter = 5
        0x3E, 0x00,       // MVI A,00     → sum = 0

        // LOOP:
        0x86,             // ADD M        → A += [HL]
        0x23,             // INX H        → HL++
        0x05,             // DCR B        → B--
        0xC2, 0x06, 0x00, // JNZ LOOP     → jump back

        // STORE RESULT
        0x32, 0x00, 0x30, // STA 3000     → store result

        0x76 // HLT
    };
    load_program(program, sizeof(program));
    while (1)
    {
        uint8_t opcode = fetch(&cpu);
        execute(&cpu, opcode);
        printf("OP=%02X ", opcode);
        log_cpu(&cpu);
    }
    printf("%04d\n", cpu.A);
    return 0;
}
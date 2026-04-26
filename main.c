#include "cpu.h"
#include "memory.h"
#include "instructions.h"

int main(void) {
    CPU cpu = {0}; // Initialize CPU state
    cpu.SP = 0xFFFF;
    uint8_t program[] = {
    0x06, 0x20, // B = 0x20
    0x0E, 0x00, // C = 0x00  → address 0x2000
    0x3E, 0x55, // A = 0x55
    0x02,       // STAX B
    0x3E, 0x00, // A = 0x00
    0x0A,       // LDAX B
    0x76        // HLT
};
    load_program(program, sizeof(program));
    while (1) {
        uint8_t opcode = fetch(&cpu);
        execute(&cpu, opcode);
        printf("OP=%02X ", opcode);
        log_cpu(&cpu);
    }
    printf("%04d\n", cpu.A);
    return 0;
}
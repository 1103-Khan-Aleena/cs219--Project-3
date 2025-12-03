#ifndef CPU_H
#define CPU_H

#include <string>
#include <vector>
#include <cstdint>
#include "instr.h"
using namespace std;

// The CPU memory starts at this address in our simulation
const uint32_t MEM_BASE = 0x100; // start of CPU memory

struct Flags {
    bool N = false;
    bool Z = false;
    bool C = false;
    bool V = false;
};

class CPU {
public:
    CPU();

    // Check if an address is in memory range
    bool inMemRange(uint32_t addr, int &index) const;

    // Get the value of operand 2 for an instruction
    uint32_t getOp2Value(const Instruction &ins) const;

    // Update the flags after addition, subtraction, or logical operation
    void updateFlagsAdd(uint32_t A, uint32_t B, uint32_t result);
    void updateFlagsSub(uint32_t A, uint32_t B, uint32_t result);
    void updateFlagsLogical(uint32_t result);

    // Check if the condition for an instruction holds
    bool condHolds(const string &cond) const;

    // Print CPU state for debugging
    void printState(const Instruction &ins) const;

    // Run a program (list of instructions)
    void run(const vector<Instruction> &program);

    uint32_t regs[12];
    uint32_t mem[5];
    Flags nzcv;
};

// Decode a string opcode into base, condition, and setsS flag
void decodeOpcode(const string &opcode_in, string &base,string &cond, bool &setsS);

// Convert base string to OpType
OpType opFromBase(const string &base);

#endif

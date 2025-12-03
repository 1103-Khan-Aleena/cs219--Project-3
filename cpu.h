#ifndef CPU_H
#define CPU_H

#include <string>
#include <vector>
#include <cstdint>
#include "instr.h"
using namespace std;

// The CPU memory starts at this address in our simulation
const uint32_t MEM_BASE = 0x100; //start of CPU memory

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

    //update the flag after add, sub, or any op
    void updateFlagsAdd(uint32_t A, uint32_t B, uint32_t result);
    void updateFlagsSub(uint32_t A, uint32_t B, uint32_t result);
    void updateFlagsLogical(uint32_t result);

    //check if the condition for an instruction holds
    bool condHolds(const string &cond) const;

    //print CPU state for debugging
    void printState(const Instruction &ins) const;

    //run the instrutions
    void run(const vector<Instruction> &program);

    uint32_t regs[12];
    uint32_t mem[5];
    Flags nzcv;
};

//decode a string opcode into base, cond, and setsS flag
void decodeOpcode(const string &opcode_in, string &base,string &cond, bool &setsS);

//convert base string to OpType
OpType opFromBase(const string &base);

#endif

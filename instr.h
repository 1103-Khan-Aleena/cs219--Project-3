#ifndef INSTR_H
#define INSTR_H

#include <string>
#include <cstdint>
using namespace std;

//lists out all the operation types that our CPU can do
enum class OpType {
    INVALID, // Not a valid instruction
    NOP,     // No operation
    ADD,     // Addition
    SUB,     // Subtraction
    AND,     // Bitwise AND
    ORR,     // Bitwise OR
    EOR,     // Bitwise XOR
    LSL,     // Logical shift left
    LSR,     // Logical shift right
    MOV,     // Move value
    MVN,     // Move NOT (bitwise NOT)
    LDR,     // Load from memory
    STR,     // Store to memory
    CMP,     // Compare
    BEQ      // Branch if equal
};


//Operand 2:can be a register or an immediate value
struct Op2 {
    bool isImmediate = false; // True if immediate value
    uint32_t imm = 0;         // Immediate value
    int reg = -1;             // Register number if not immediate
};


//instruction structure
struct Instruction {
    OpType op = OpType::INVALID; // The operation
    string cond;            // Condition code
    bool setsFlags = false;      //does update the flags orr
    string label;           // abel for branching
    bool hasLabel = false;       //True if instruction has a label
    string args;            //ops as raw text
    int Rn = -1;                 //first register operand
    int Rd = -1;                 //destination register
    Op2 op2;                     //second operand
    string raw;             //full raw instruction text
    int branchTarget = -1;       //target instruction index for BEQ
};

#endif

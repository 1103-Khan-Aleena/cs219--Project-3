#include "cpu.h"
#include "helpers.h"
#include <iostream>
#include <iomanip> //referenced website cplusplus.com "<iomanip>"
using namespace std;

// CPU constructor
CPU::CPU() {
    // Initialize registers and memory to 0
    for (int i = 0; i < 12; ++i) {
        regs[i] = 0;
    }
    for (int i = 0; i < 5; ++i) {
        mem[i] = 0;
    }

    // Initialize flags
    nzcv = Flags{};
}

// Check if address is in memory range
bool CPU::inMemRange(uint32_t addr, int &index) const {
    if (addr < MEM_BASE) {
        return false;
    }
    uint32_t offset = addr - MEM_BASE;
    // Only word-aligned addresses allowed
    if (offset % 4 != 0) {
        return false;
    }
    index = static_cast<int>(offset / 4);
    return (index >= 0 && index < 5);
}

// Get the value of operand 2
uint32_t CPU::getOp2Value(const Instruction &ins) const {
    if (ins.op2.isImmediate) {
        return ins.op2.imm;
    }
    if (ins.op2.reg >= 0 && ins.op2.reg < 12) {
        return regs[ins.op2.reg];
    }
    return 0;
}

// Update flags for ADD
void CPU::updateFlagsAdd(uint32_t A, uint32_t B, uint32_t result) {
    uint64_t sum64 = static_cast<uint64_t>(A) + static_cast<uint64_t>(B);
    bool carryFlag = ((sum64 >> 32) & 1);
    bool negativeFlag = (result >> 31) & 1;
    bool zeroFlag = (result == 0);
    bool overflowFlag = (((A ^ ~B) & (A ^ result)) >> 31) & 1;

    nzcv.C = carryFlag;
    nzcv.N = negativeFlag;
    nzcv.Z = zeroFlag;
    nzcv.V = overflowFlag;
}

// Update flags for SUB/CMP
void CPU::updateFlagsSub(uint32_t A, uint32_t B, uint32_t result) {
    bool carryFlag = (A >= B);
    bool negativeFlag = (result >> 31) & 1;
    bool zeroFlag = (result == 0);
    bool overflowFlag = (((A ^ B) & (A ^ result)) >> 31) & 1;

    nzcv.C = carryFlag;
    nzcv.N = negativeFlag;
    nzcv.Z = zeroFlag;
    nzcv.V = overflowFlag;
}

// Update flags for logical operations
void CPU::updateFlagsLogical(uint32_t result) {
    bool negativeFlag = (result >> 31) & 1;
    bool zeroFlag = (result == 0);

    nzcv.N = negativeFlag;
    nzcv.Z = zeroFlag;
    // Carry and Overflow dont change!!!

}

// Check if the condition of instruction holds
bool CPU::condHolds(const string &cond) const {
    bool isZeroFlagSet = nzcv.Z;
    bool isNegativeFlagSet = nzcv.N;
    bool isCarryFlagSet = nzcv.C;
    bool isOverflowFlagSet = nzcv.V;

    if (cond.empty()) return true;
    if (cond == "EQ") {
        if (isZeroFlagSet == true) return true;
        else return false;
    }
    if (cond == "NE") {
        if (isZeroFlagSet == false) return true;
        else return false;
    }
    if (cond == "GT") {
        if ((isZeroFlagSet == false) && (isNegativeFlagSet == isOverflowFlagSet)) return true;
        else return false;
    }
    if (cond == "GE") {
        if (isNegativeFlagSet == isOverflowFlagSet) return true;
        else return false;
    }
    if (cond == "LT") {
        if (isNegativeFlagSet != isOverflowFlagSet) return true;
        else return false;
    }
    if (cond == "LE") {
        if ((isZeroFlagSet == true) || (isNegativeFlagSet != isOverflowFlagSet)) return true;
        else return false;
    }
    return false;
}

// Print the current CPU state
void CPU::printState(const Instruction &ins) const {
    cout << ins.raw << "\n";
    // Print registers
    cout << "Register array:\n";
    for (int i = 0; i < 12; i++) {
        cout << "R" << i << "=" << toHex(regs[i]);
        if (i < 11) cout << " ";
    }
    cout << "\n";

    // Print flags
    cout << "NZCV: "
         << (nzcv.N ? '1' : '0')
         << (nzcv.Z ? '1' : '0')
         << (nzcv.C ? '1' : '0')
         << (nzcv.V ? '1' : '0') << "\n";
    // Print memory
    cout << "Memory array:\n";
    for (int i = 0; i < 5; i++) {
        if (mem[i] == 0) cout << "___";
        else cout << toHex(mem[i]);
        if (i < 4) cout << ",";
    }
    cout << "\n";
}
// Decode an opcode into base, condition, and setsS
void decodeOpcode(const string &opcode_in, string &base, string &cond, bool &setsS) {
    string opcode = opcode_in;
    base.clear();
    cond.clear();
    setsS = false;
    if (opcode.empty()) return;
    if (opcode.back() == 'S') {
        setsS = true;
        opcode.pop_back();
    }
    const string conds[] = {"EQ", "NE", "GT", "GE", "LT", "LE"};

    for (const string &c : conds) {
        if (opcode.size() > c.size() &&
            opcode.compare(opcode.size() - c.size(), c.size(), c) == 0) {
            cond = c;
            base = opcode.substr(0, opcode.size() - c.size());
            return;
        }
    }
    base = opcode;
}
// Convert base string to OpType
OpType opFromBase(const string &base) {
    if (base == "ADD") return OpType::ADD;
    if (base == "SUB") return OpType::SUB;
    if (base == "MOV") return OpType::MOV;
    if (base == "AND") return OpType::AND;
    if (base == "ORR") return OpType::ORR;
    if (base == "EOR") return OpType::EOR;
    if (base == "LSL") return OpType::LSL;
    if (base == "LSR") return OpType::LSR;
    if (base == "MVN") return OpType::MVN;
    if (base == "LDR") return OpType::LDR;
    if (base == "STR") return OpType::STR;
    if (base == "CMP") return OpType::CMP;
    if (base == "BEQ") return OpType::BEQ;
    if (base == "NOP") return OpType::NOP;

    return OpType::INVALID;
}
// Run a program (list of instructions)

void CPU::run(const vector<Instruction> &program) {
    int programCounter = 0;
    int programSize = static_cast<int>(program.size());

    while (programCounter < programSize) {
        const Instruction &ins = program[programCounter];
        // Handle label-only instruction
        if (ins.op == OpType::INVALID && ins.hasLabel) {
            Instruction tmp = ins;
            tmp.raw = ins.label + ":";
            printState(tmp);
            programCounter++;
            continue;
        }

        // Evaluate condition
        bool shouldExecute = condHolds(ins.cond);
        if (!shouldExecute) {
            printState(ins);
            programCounter++;
            continue;
        }

        // Execute instruction
        switch (ins.op) {
            case OpType::ADD: {
                uint32_t firstOperand = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                uint32_t secondOperand = getOp2Value(ins);
                uint32_t result = static_cast<uint32_t>(
                    (static_cast<uint64_t>(firstOperand) + static_cast<uint64_t>(secondOperand)) & 0xFFFFFFFFu);
                if (ins.Rd >= 0) regs[ins.Rd] = result;
                if (ins.setsFlags) updateFlagsAdd(firstOperand, secondOperand, result);
                break;
            }
            case OpType::SUB: {
                uint32_t firstOperand = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                uint32_t secondOperand = getOp2Value(ins);
                uint32_t result = static_cast<uint32_t>(
                    (static_cast<uint64_t>(firstOperand) - static_cast<uint64_t>(secondOperand)) & 0xFFFFFFFFu);
                if (ins.Rd >= 0) regs[ins.Rd] = result;
                if (ins.setsFlags) updateFlagsSub(firstOperand, secondOperand, result);
                break;
            }
            case OpType::AND: {
                uint32_t firstOperand = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                uint32_t secondOperand = getOp2Value(ins);
                uint32_t result = firstOperand & secondOperand;
                if (ins.Rd >= 0) regs[ins.Rd] = result;
                if (ins.setsFlags) updateFlagsLogical(result);
                break;
            }
            case OpType::ORR: {
                uint32_t firstOperand = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                uint32_t secondOperand = getOp2Value(ins);
                uint32_t result = firstOperand | secondOperand;
                if (ins.Rd >= 0) regs[ins.Rd] = result;
                if (ins.setsFlags) updateFlagsLogical(result);
                break;
            }
            case OpType::EOR: {
                uint32_t firstOperand = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                uint32_t secondOperand = getOp2Value(ins);
                uint32_t result = firstOperand ^ secondOperand;
                if (ins.Rd >= 0) regs[ins.Rd] = result;
                if (ins.setsFlags) updateFlagsLogical(result);
                break;
            }
            case OpType::LSL: {
                uint32_t value = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                uint32_t shiftAmount = getOp2Value(ins) & 0x1F;
                uint32_t result = (shiftAmount >= 32) ? 0 : (value << shiftAmount);
                if (ins.Rd >= 0) regs[ins.Rd] = result;
                if (ins.setsFlags) {
                    if (shiftAmount != 0) nzcv.C = ((value >> (32 - shiftAmount)) & 1);
                    updateFlagsLogical(result);
                }
                break;
            }
            case OpType::LSR: {
                uint32_t value = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                uint32_t shiftAmount = getOp2Value(ins) & 0x1F;
                uint32_t result = (shiftAmount >= 32) ? 0 : (value >> shiftAmount);
                if (ins.Rd >= 0) regs[ins.Rd] = result;
                if (ins.setsFlags) {
                    if (shiftAmount != 0) nzcv.C = ((value >> (shiftAmount - 1)) & 1);
                    updateFlagsLogical(result);
                }
                break;
            }
            case OpType::MOV: {
                uint32_t value = getOp2Value(ins);
                if (ins.Rd >= 0) regs[ins.Rd] = value;
                if (ins.setsFlags) updateFlagsLogical(value);
                break;
            }
            case OpType::MVN: {
                uint32_t value = ~getOp2Value(ins);
                if (ins.Rd >= 0) regs[ins.Rd] = value;
                if (ins.setsFlags) updateFlagsLogical(value);
                break;
            }
            case OpType::LDR: {
                uint32_t address = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                int memoryIndex = -1;
                if (inMemRange(address, memoryIndex)) {
                    if (ins.Rd >= 0) regs[ins.Rd] = mem[memoryIndex];
                }
                break;
            }
            case OpType::STR: {
                uint32_t address = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                int memoryIndex = -1;
                if (inMemRange(address, memoryIndex)) {
                    if (ins.Rd >= 0) mem[memoryIndex] = regs[ins.Rd];
                }
                break;
            }
            case OpType::CMP: {
                uint32_t firstOperand = (ins.Rn >= 0 ? regs[ins.Rn] : 0);
                uint32_t secondOperand = getOp2Value(ins);
                uint32_t result = static_cast<uint32_t>(
                    (static_cast<uint64_t>(firstOperand) - static_cast<uint64_t>(secondOperand)) & 0xFFFFFFFFu);
                updateFlagsSub(firstOperand, secondOperand, result);
                break;
            }
            case OpType::BEQ: {
                bool isZeroFlagSet = nzcv.Z;
                if (isZeroFlagSet == true) {
                    if (ins.branchTarget >= 0) {
                        programCounter = ins.branchTarget;
                        printState(ins);
                        continue;
                    }
                }
                break;
            }
            default: break;
        }

        printState(ins);
        programCounter++;
    }
}

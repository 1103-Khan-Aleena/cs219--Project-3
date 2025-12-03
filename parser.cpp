#include "parser.h"
#include "cpu.h"
#include "helpers.h"
#include <vector>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;


//remove leading and trailing whitespace
string trimString(const string &input) {
    size_t startIndex = input.find_first_not_of(" \t");
    if (startIndex == string::npos) return "";

    size_t endIndex = input.find_last_not_of(" \t");
    return input.substr(startIndex, endIndex - startIndex + 1);
}

//parse a register string like R0 or [R1]
//returns -1 if invalid

int parseRegister(const string &operand) {
    string operandCopy = operand;

    // Remove brackets if present
    if (!operandCopy.empty() && operandCopy.front() == '[' && operandCopy.back() == ']') {
        operandCopy = operandCopy.substr(1, operandCopy.size() - 2);
    }

    if (operandCopy.size() >= 2 && (operandCopy[0] == 'R' || operandCopy[0] == 'r')) {
        try {
            return stoi(operandCopy.substr(1));
        } catch (...) {
            return -1;
        }
    }
    return -1;
}


//parse operand 2 (immediate or register)
Op2 parseOperand2(const string &operandStr) {
    Op2 operand;
    if (operandStr.empty()) return operand;

    // Immediate starts with '#' or is a number
    if (operandStr[0] == '#') {
        operand.isImmediate = true;
        operand.imm = parseNumber(operandStr);
        return operand;
    }

    if ((operandStr.size() > 1 && operandStr[0] == '0' && (operandStr[1] == 'x' || operandStr[1] == 'X')) || 
         (operandStr.size() > 0 && isdigit((unsigned char)operandStr[0]))) {
        operand.isImmediate = true;
        operand.imm = parseNumber(operandStr);
        return operand;
    }

    // Otherwise, treat as register
    int regNumber = parseRegister(operandStr);
    if (regNumber >= 0) {
        operand.isImmediate = false;
        operand.reg = regNumber;
    }

    return operand;
}

//main function to parse program lines into instructions
vector<Instruction> parseProgram(const vector<string> &programLines) {
    vector<Instruction> parsedInstructions;

    // First pass: parse each line
    for (const string &originalLine : programLines) {
        string line = trimString(originalLine);
        if (line.empty()) continue;

        Instruction instruction;
        instruction.raw = line;

        // Split first word (possible label or opcode) from rest
        size_t firstSpaceIndex = line.find_first_of(" \t");
        string firstWord = (firstSpaceIndex == string::npos) ? line : line.substr(0, firstSpaceIndex);
        string restOfLine = (firstSpaceIndex == string::npos) ? "" : trimString(line.substr(firstSpaceIndex + 1));

        string opcodeCandidate;
        string operandsString;

        //check if first word is a label or opcode
        string baseOpcode, condition;
        bool setsFlags = false;
        decodeOpcode(firstWord, baseOpcode, condition, setsFlags);

        if (opFromBase(baseOpcode) == OpType::INVALID) {
            //treat first word as label
            instruction.label = firstWord;
            instruction.hasLabel = true;

            if (!restOfLine.empty()) {
                size_t spaceAfterOpcode = restOfLine.find_first_of(" \t");
                opcodeCandidate = (spaceAfterOpcode == string::npos) ? restOfLine : restOfLine.substr(0, spaceAfterOpcode);
                operandsString = (spaceAfterOpcode == string::npos) ? "" : trimString(restOfLine.substr(spaceAfterOpcode + 1));
            }
        } else {
            opcodeCandidate = firstWord;
            operandsString = restOfLine;
        }

        //decode opcode
        if (!opcodeCandidate.empty()) {
            decodeOpcode(opcodeCandidate, baseOpcode, condition, setsFlags);
            instruction.op = opFromBase(baseOpcode);
            instruction.cond = condition;
            instruction.setsFlags = setsFlags;
        } else {
            instruction.op = OpType::INVALID;
        }

        instruction.args = operandsString;

        // split operands by comma
        vector<string> operands;
        string currentOperand;
        for (char c : operandsString) {
            if (c == ',') {
                operands.push_back(trimString(currentOperand));
                currentOperand.clear();
            } else {
                currentOperand.push_back(c);
            }
        }
        if (!currentOperand.empty()) {
            operands.push_back(trimString(currentOperand));
        }

        //assign registers and operand2 based on opcode
        if (instruction.op == OpType::MOV || instruction.op == OpType::MVN) {
            if (operands.size() >= 1) instruction.Rd = parseRegister(operands[0]);
            if (operands.size() >= 2) instruction.op2 = parseOperand2(operands[1]);
        } 
        else if (instruction.op == OpType::ADD || instruction.op == OpType::SUB || 
                 instruction.op == OpType::AND || instruction.op == OpType::ORR || 
                 instruction.op == OpType::EOR || instruction.op == OpType::LSL || 
                 instruction.op == OpType::LSR) {
            if (operands.size() >= 1) instruction.Rd = parseRegister(operands[0]);
            if (operands.size() >= 2) instruction.Rn = parseRegister(operands[1]);
            if (operands.size() >= 3) instruction.op2 = parseOperand2(operands[2]);
        } 
        else if (instruction.op == OpType::CMP) {
            instruction.setsFlags = true;
            if (operands.size() >= 1) instruction.Rn = parseRegister(operands[0]);
            if (operands.size() >= 2) instruction.op2 = parseOperand2(operands[1]);
        } 
        else if (instruction.op == OpType::LDR || instruction.op == OpType::STR) {
            if (operands.size() >= 1) instruction.Rd = parseRegister(operands[0]);
            if (operands.size() >= 2) instruction.Rn = parseRegister(operands[1]);
        } 
        else if (instruction.op == OpType::BEQ) {
            if (operands.size() >= 1) instruction.label = operands[0];
        }

        parsedInstructions.push_back(instruction);
    }

    //fix BEQ branch targets by label positions
    for (size_t i = 0; i < parsedInstructions.size(); ++i) {
        Instruction &ins = parsedInstructions[i];
        if (ins.op == OpType::BEQ && !ins.label.empty()) {
            for (size_t j = 0; j < parsedInstructions.size(); ++j) {
                if (parsedInstructions[j].hasLabel && parsedInstructions[j].label == ins.label) {
                    ins.branchTarget = static_cast<int>(j);
                    break;
                }
            }
        }
    }

    return parsedInstructions;
}

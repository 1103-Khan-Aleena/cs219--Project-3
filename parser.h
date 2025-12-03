#ifndef PARSER_H
#define PARSER_H

#include "instr.h"
#include <vector>
#include <string>

using namespace std;

// Parse a list of program lines into instructions
vector<Instruction> parseProgram(const vector<string> &lines);

#endif

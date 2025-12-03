#include <iostream>
// main.cpp
// Author: Aleena Khan 
#include "cpu.h"
#include "parser.h"
#include "helpers.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

int main(int argc, char **argv) {
    //the input file 
    string inputFileName;
    if (argc > 1) {
        inputFileName = argv[1]; //could be any file name but for this project were gonns stick wiht pp3 input
    } else {
        inputFileName = "PP3_input.txt"; //default
    }

    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error: Unable to open file: " << inputFileName << endl;
        return 1;
    }

    // readng the non-empty lines
    vector<string> programLines;
    string line;
    while (getline(inputFile, line)) {
        string trimmedLine = trim(line);
        if (!trimmedLine.empty()) {
            programLines.push_back(trimmedLine);
        }
    }

    // Parse program into instructions
    vector<Instruction> programInstructions = parseProgram(programLines);

    //cmake CPU and run program
    CPU myCpu;
    myCpu.run(programInstructions);

    return 0;
}

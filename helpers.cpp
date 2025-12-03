#include "helpers.h"
#include <sstream>
#include <cctype>
#include <string>
#include <iostream>
using namespace std;
// Convert a string into a number 
uint32_t parseNumber(const string &input) {
    string str = input;
    // Remove # if its there
    if (!str.empty() && str[0] == '#') {
        str = str.substr(1);  //remove the first char
    }
    //Convert string to unsigned long 
    uint32_t value = static_cast<uint32_t>(stoul(str, nullptr, 0));

    return value;
}
// Convert an integer to a hexadecimal string
string toHex(uint32_t value) {
    ostringstream out;
    out << "0x";             //hex
    out << hex;         //use hexadecimal
    out << nouppercase; //lowercase letters for a-f
    out << value;

    return out.str();
}

// Remove leading and trailing whitespace string a string
string trim(const string &input) {
    size_t start = 0;
    size_t end = input.size();

    //Move start index past all leading whitespace
    while (start < end && isspace(static_cast<unsigned char>(input[start]))) {
        start++;
    }

    //Move end index before trailing whitespace
    while (end > start && isspace(static_cast<unsigned char>(input[end - 1]))) {
        end--;
    }
    //Extract the substring without leading/trailing spaces
    string result = input.substr(start, end - start);

    return result;
}

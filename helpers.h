#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <cstdint>
using namespace std;

// Convert a string to a number
uint32_t parseNumber(const string &inputString);


// Convert an integer to a hexadecimal string
string toHex(uint32_t number);


// Remove leading and trailing whitespace from a string
string trim(const string &inputString);

#endif

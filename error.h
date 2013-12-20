#pragma once
#include "datatypes.h


struct error {
    int64 data;
    
    error()      : code(0) {}
    error(int n) : code(n) {}
    operator int () { return code; }
    int operator = (int other) { return code = other; }
}
std::ostream& operator<< (std::ostream& o, error err);

const error OK = 0;
#pragma once
#include "builtin.h"

namespace alex {
namespace syscall {

struct error {
    int code;
    
    error()      : code(0) {}
    error(int n) : code(n) {}
    operator int () { return code; }
    int operator = (int other) { return code = other; }
    operator alex::error ();
    
};
std::ostream& operator<< (std::ostream& o, error err);

const error OK = 0;
    
int open(str path, int mode, uint32 perm, error& err);



}
}
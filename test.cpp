// #include <csignal>
// #include <execinfo.h>
// #include <unistd.h>
//#include <ostream>
// #include <stdio.h>
#include <iostream>
#include <stdio.h>

#include "builtin.h"
#include "include/os"

#include "print.h"




int main() {
    error err;
    auto file = os::open("test.cpp2", err);
    if (err) {
        warn "Error opening file: %s" % err;
    } else {
        warn "No error: %s" % err;
    }
    
    bytebuf buf(25);
    
    //err = OK;
    print "%x %s\n" % (uint64)err.charp, (bool)err;
    
    return 0;
}
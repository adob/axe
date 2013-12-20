#include <csignal>
#include <execinfo.h>
#include <unistd.h>
#include <ostream>
#include <stdio.h>
#include <iostream>

#include "datatypes.h"
#include "str.h"
#include "assert.h"
#include "io/io.h"
#include "os/file.h"
#include "syscall/syscall.h"

#include "print.h"

using namespace alex;

// void handler() {
//     void *array[10];
//     size_t size;
//     
//     // get void*'s for all entries on the stack
//     size = backtrace(array, 10);
//     
//     // print out all the frames to stderr
//     fprintf(stderr, "Error: signal\n");
//     backtrace_symbols_fd(array, size, STDERR_FILENO);
//     exit(1);
// }

int main() {
    //std::signal(SIGSEGV, handler);   // install our handler
    //std::set_terminate(handler);
    
    os::error err;
    auto file = os::open("test.cpp2", err);
    if (err) {
        print "Got err: %s" % err;
    }
    fixedbuf<16> buf;
    file.read(buf);
    
    os::File *file2 = nullptr;
    file2->read(buf);
    
    
    //str x = "foo";
    //printf("%s\n", x.data);
    //print "x = %s" % x;
}
#include <vector>
#import "alloc.h"

#ifndef AXE_BACKTRACE_H
#define AXE_BACKTRACE_H
namespace axe {
    struct Allocator;
    struct Backtrace {
        std::vector<void*> addrs;
        
        str string(Allocator&) const;
    } ;
}
#endif
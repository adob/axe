#include <vector>


#ifndef AXE_BACKTRACE_H
#define AXE_BACKTRACE_H
namespace axe {
    struct Allocator;
    struct strref;
    using str = strref;
    
    struct Backtrace {
        std::vector<void*> addrs;
        
        str string(Allocator&) const;
    } ;
}
#endif
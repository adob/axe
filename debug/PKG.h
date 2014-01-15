#include <vector>
#import <axe/core.h>

namespace axe {
namespace debug {
    
    struct LineInfo {
        std::string  funcname;
        std::string  symbolname;
        std::string  filepath;
        uint         lineno;
    } ;
    
    void init();
    
    LineInfo addr2line(void* addr, errorparam err = {});
    std::string demangle(str);
    void crash_handler();
    Backtrace backtrace(int offset=0);
    void print_backtrace(int offset=0);
}
}
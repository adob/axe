#include <vector>
#import <axe/debug/addr2line.h>
#import <axe/buffer.h>
#import <axe/strings/strings.h>
#import <axe/fmt/fmt.h>
#import "backtrace.h"

namespace axe { 
    
    
    str Backtrace::string(Allocator& alloc) const {
        std::vector<str> lines;
        
        for (void *ptr : addrs) {
            debug::LineInfo info = debug::addr2line(ptr);
            Str s(alloc);
            
            s += fmt::sprintf("  %#.12x", (uintptr)ptr);
            if (info.funcname != "??") {
                s += fmt::sprintf(" in %s", info.funcname);
            }
            if (info.filepath != "??") {
                s += fmt::sprintf(" at %s:%d", info.filepath, info.lineno);
            }
            lines.push_back(s);
        }
        
        return strings::join(alloc, lines, "\n");
    }
    
}
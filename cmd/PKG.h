#include <initializer_list>
#include <axe/core.h>

namespace axe {
namespace cmd {
    
    struct Cmd {
        str       path;
        list<str> args;
        
        Allocator& alloc;
        
        Cmd(str name, std::initializer_list<str> args, Allocator& alloc) : alloc(alloc) {
            path = name;
            List<str> argslist(alloc);
            
            for (str arg : args) {
                argslist.make(str);
            }
            
            this->args = argslist;
        }
        
        str output(errorparam err = {}) {
            
        }
    } ;
}
}
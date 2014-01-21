#include <initializer_list>
#import <axe/str.h>
#import <axe/errorparam.h>
#import <axe/adt/list.h>

namespace axe {
    namespace cmd {
        
    //     struct Cmd {
    //         str       path;
    //         list<str> args;
    //         
    //         Allocator& alloc;
    //         
    //         Cmd(str name, std::initializer_list<str> args, Allocator& alloc) : alloc(alloc) {
    //             path = name;
    //             List<str> argslist(alloc);
    //             
    //             for (str arg : args) {
    //                 argslist.add(arg);
    //             }
    //             
    //             this->args = argslist;
    //         }
    //         
    //         str output(errorparam err = {}) {
    //             return "";
    //         }
    //     } ;
    }
}
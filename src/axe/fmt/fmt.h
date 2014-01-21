#include <string>

#import <axe/str.h>
//#import <axe/io/PKG.h>
//#import <axe/utf8/PKG.h>

namespace axe {
    namespace fmt {
        
        
        template <typename... Args>
        void printf(str format, const Args &... args);
        
        template <typename... Args>
        void fprintf(FILE*, str format, const Args & ... args);
        
        template <typename... Args>
        str sprintf(Allocator&, str format, const Args & ... args);
        
        template <typename T, typename... Args>
        void writef(T&, str format, const Args & ... args);
        
        template <typename... Args>
        std::string sprintf(str format, const Args & ... args);
        
        template <typename T>
        str stringify(T const& t, Allocator& alloc);
        
        template <typename T>
        std::string stringify(T const& t);
        
    }
}

#import <axe/fmt/fmt.inlines.h>

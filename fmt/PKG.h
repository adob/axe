#include <string>

#import <axe/core.h>
#import <axe/io/PKG.h>
#import <axe/utf8/PKG.h>

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
    
    str quote(str s, Allocator& alloc, bool ascii_only=false);
    str quote(rune r, Allocator& alloc, bool ascii_only=false);
    str quote(char c, Allocator& alloc, bool ascii_only=false);
    
    // IsPrint reports whether the rune is defined as printable by Go, with
    // the same definition as unicode.IsPrint: letters, numbers, punctuation,
    // symbols and ASCII space.
    bool is_printable(rune r);
    
    
}
}

#import <axe/fmt/fmt.inlines.h>

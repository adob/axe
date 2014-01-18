#import <axe/str.h>
namespace axe {
    namespace fmt {
        str quote(str s, Allocator& alloc, bool ascii_only=false);
        str quote(rune r, Allocator& alloc, bool ascii_only=false);
        str quote(char c, Allocator& alloc, bool ascii_only=false);
        
        // IsPrint reports whether the rune is defined as printable by Go, with
        // the same definition as unicode.IsPrint: letters, numbers, punctuation,
        // symbols and ASCII space.
        bool is_printable(rune r);
    }
}
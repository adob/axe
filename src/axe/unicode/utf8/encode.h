#import <axe/typedefs.h>
#import <axe/alloc.h>
#import <axe/str.h>

namespace axe {
    namespace unicode {
        namespace utf8 {
            /// encode(r, b) writes into b the UTF-8 encoding
            /// of the rune r. The resulting string is returned. If b is not large enough, an exception
            /// is raised. No more than UTFMax bytes are written into b.
            str encode(rune r, bufref b);
            /// encode(r, alloc) allocates a string using the allocator alloc, writes the UTF-8
            /// encoding of the rune r, and returns the resulting string.
            str encode(rune r, Allocator& alloc);
        }
    }
}
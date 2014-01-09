#include <axe/fmt.h>
#include "isprint.h"
#include "unicode/utf8.h"

namespace axe {
namespace fmt {

constexpr str lowerhex("0123456789abcdef");
    
str quote_with(str s, char quote, bool ascii_only , Allocator& alloc) {
    Buf buf(alloc, 3*len(s)/2);
    buf.append(quote);
    for (int width = 0; len(s) > 0; s = s(width)) {
        byte b = s[0];
        rune r = b;
        width = 1; 
        if (r >= utf8::RuneSelf) {
            r = utf8::decode(s, width);
        } ;
        if (width == 1 && r == utf8::RuneError) {
            buf.append("\\x", lowerhex[b>>4] , lowerhex[b&0xF]);
            continue;
        }
        if (r == (rune) quote || r == '\\') { // always backslashed
            buf.append('\\', r);
            continue;
        }
        if (ascii_only) {
            if (r < utf8::RuneSelf && is_printable(r)) {
                buf.append((char) r);
                continue;
            }
        } else if (is_printable(r)) {
            buf.append(r);
            continue;
        }
        switch (r) {
            case '\a': buf.append("\\a"); break;
            case '\b': buf.append("\\b"); break;
            case '\f': buf.append("\\f"); break;
            case '\n': buf.append("\\r"); break;
            case '\r': buf.append("\\r"); break;
            case '\t': buf.append("\\t"); break;
            case '\v': buf.append("\\v"); break;
            default:
                if (r < ' ') {
                    buf.append("\\x", lowerhex[b>>4], lowerhex[b&0xF]);
                } else if (r > utf8::MaxRune) {
                    buf.append("\\u");
                    for (int s = 12; s >= 0; s -= 4) {
                        buf.append(lowerhex[ (r>>s) & 0xF ]);
                    }
                } else {
                    buf.append("\\U");
                    for (int s = 28; s >= 0; s -= 4) {
                        buf.append(lowerhex[ (r>>s) & 0xF ]);
                    }
                }
        }
    }
    buf.append(quote);
    return buf;
}

// bsearch returns the smallest i such that a[i] >= x.
// If there is no such i, bsearch returns len(a).
template <typename T>
size bsearch(array<T> a, uint32 x) {
    size i = 0, j = len(a);
    while (i < j) {
        size h = i + (j-i)/2;
        if (a[h] < x) {
            i = h + 1;
        } else {
            j = h;
        }
    }
    return i;
}

str quote(str s, Allocator& alloc, bool ascii_only) {
    return quote_with(s, '"', ascii_only, alloc);
}

str quote(rune r, Allocator& alloc, bool ascii_only) {
    Array<char, utf8::UTFMax> buf;
    return quote_with(utf8::encode(r, buf), '\'', ascii_only, alloc);
}

str quote(char c, Allocator& alloc, bool ascii_only) {
    Array<char, 1> buf {c};
    return quote_with(buf, '\'', ascii_only, alloc);
}

bool is_printable(rune r) {
    // Fast check for Latin-1
    if (r <= 0xFF) {
        if (0x20 <= r && r <= 0x7E) {
            // All the ASCII is printable from space through DEL-1.
            return true;
        }
        if (0xA1 <= r && r <= 0xFF) {
            // Similarly for ¡ through ÿ...
            return r != 0xAD; // ...except for the bizarre soft hyphen.
        }
        return false;
    }
    
    // Same algorithm, either on uint16 or uint32 value.
    // First, find first i such that isPrint[i] >= x.
    // This is the index of either the start or end of a pair that might span x.
    // The start is even (isPrint[i&^1]) and the end is odd (isPrint[i|1]).
    // If we find x in a range, make sure x is not in isNotPrint list.
    
    if (r < (1<<16)) {
        size i = bsearch(IsPrint16, r);
        if (i > len(IsPrint16) || r < IsPrint16[i & ~1] || IsPrint16[i|1] < r) {
            return false;
        }
        size j = bsearch(IsNotPrint16, r);
        return j >= len(IsNotPrint16) || IsNotPrint16[j] != r;
    }
    
    size i = bsearch(IsPrint32, r);
    if (i > len(IsPrint32) || r < IsPrint32[i & ~1] || IsPrint32[i|1] < r) {
        return false;
    }
    if (r >= 0x20000) {
        return true;
    }
    r -= 0x10000;
    size j = bsearch(IsNotPrint32, r);
    return j >= len(IsNotPrint32) || IsNotPrint32[j] != r;

}
    
}
}
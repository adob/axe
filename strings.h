#pragma once

#include <axe/core.h>
namespace axe {
namespace strings {
    size index(str haystack, str needle);
    inline size index(str s, char c) { 
        const char *p = (const char *) memchr(s.data, c, s.len);
        if (p) {
            return p - s.data;
        } else {
            return -1;
        }
    }
    
    inline size rindex(str s, char c) { 
        const char *p = (const char *) memrchr(s.data, c, s.len);
        if (p) {
            return p - s.data;
        } else {
            return -1;
        }
    }
    
    str to_lower_ascii(str s, Allocator& a);
    
    constexpr bool has_prefix(str s, str prefix) {
        return len(s) >= len(prefix) && s(0,len(prefix)) == prefix;
    }
    constexpr bool has_prefix(str s, char prefix) {
        return len(s) >= 1 && s[0] == prefix;
    }
}
}
#import <axe/core.h>
#import <axe/utf8/PKG.h>


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
    
    inline size index(str s, rune r) {
        if (r < utf8::RuneSelf) {
            return index(s, (char) r);
        }
        
        size start = 0;
        while (start < len(s)) {
            int wid = 1;
            rune rr = s[start];
            if (rr >= utf8::RuneSelf) {
                rr = utf8::decode(s(start), wid);
            }
                
            if (rr == r) {
                return start;
            }
                
            start += wid;
        }
        return -1;
        
        
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
    
    template <typename Func>
    size index(str s, Func&& f, bool truth = true) {
        size start = 0;
        while (start < len(s)) {
            int wid = 1;
            rune r = s[start];
            if (r >= utf8::RuneSelf) {
                r = utf8::decode(s(start), wid);
            }
            if (f(r) == truth) {
                return start;
            }
            start += wid;
        }
        return -1;
    }
    
    template <typename Func>
    size rindex(str s, Func&& f, bool truth = true) {
        for (size i = len(s); i > 0; ) {
            int wid;
            rune r = utf8::decode_last(s(0, i), wid);
            i -= wid;
            if (f(r) == truth) {
                return i;
            }
        }
        return -1;
    }
    
    template <typename Func>
    str ltrim(str s, Func&& f) {
        size i = index(s, f, false);
        if (i == -1) {
            return "";
        }
        return s(i);
    }
    
    str ltrim(str s, str cutset);
    str rtrim(str s, str cutset);
    str trim(str s, str cutset);
    
    template <typename Func>
    str rtrim(str s, Func&& f) {
        size i = rindex(s, f, false);
        if (i != -1 && s[i] >= utf8::RuneSelf) {
            int wid;
            utf8::decode(s(i), wid);
            i += wid;
        } else {
            i++;
        }
        return s(0,i);
    }
    
    template <typename Func>
    str trim(str s, Func&& f) {
        return rtrim(ltrim(s, f), f);
    } 
    
    str trim(str s);
    str ltrim(str s);
    str rtrim(str s);
    
    template <typename T>
    std::string join(T const& t, str sep) {
        auto iter = begin(t);
        auto iterend = end(t);
        if (iter == iterend) {
            return "";
        }
        std::string s = *iter;
        ++iter;
        while (iter != iterend) {
            s += sep;
            s += *iter;
            ++iter;
        }
        return s;
    }
    
    template <typename T>
    str join(Allocator& alloc, T const& t, str sep) {
        auto iter = begin(t);
        auto iterend = end(t);
        if (iter == iterend) {
            return "";
        }
        Str s(alloc);
        s += *iter;
        ++iter;
        while (iter != iterend) {
            s += sep;
            s += *iter;
            ++iter;
        }
        return s;
    }
}
}
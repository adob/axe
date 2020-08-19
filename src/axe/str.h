#include <string>
#include <string.h>
#include <stddef.h>
#include <vector>
#include <type_traits>
#import "typedefs.h"
#import "assert.h"
#import "exceptions.h"
#import "backtrace.h"

namespace axe {
   
    //#ifdef __clang__
    //constexpr size len(const char *s) {
    //    return s[0] == '\0' ? 0 : 1 + len(s+1);
    //}
    //#else
    //constexpr size len(const char *s) {
    //    //return s[0] == '\0' ? 0 : 1 + len(s+1);
    //    return __builtin_strlen(s);
    //}
    //#endif
    
#ifndef AXE_BACKTRACE_H
#define AXE_BACKTRACE_H
struct Allocator;
struct Backtrace {
    std::vector<void*> addrs;
    
    str string(Allocator&) const;
} ;
#endif
    
#ifndef AXE_EXCEPTION_H
#define AXE_EXCEPTION_H
    struct Exception {
        Backtrace   backtrace;
        std::string        msg;
        
        Exception();
        
        str string(Allocator&) const;
    } ;
    
    namespace exceptions {
        struct BadIndex : Exception {
            BadIndex();
            BadIndex(size got);
            BadIndex(size got, size max);
        };
        
        struct BadMemAccess : Exception {
            BadMemAccess(void *ptr);
        };
        
        struct NullMemAccess : Exception {
            NullMemAccess();
        };
        
        struct AssertionFailed : Exception {
            AssertionFailed(str);
        };
    }
#endif
    
    struct TempCStr {
        char * charp;
        char buf[256];
        
        TempCStr(const char *data, size len);
        operator const char * ();
        ~TempCStr();
    } ;
    
//     namespace internal {
//         template <typename T>
//         struct ConvHelper {
//             
//         }
//     }

    struct strref {
        const char * data;
        size         len;
        
        constexpr strref() : data(nullptr), len(0) {}
        
        constexpr strref(const void *s) : data((const char *)s), len(s != nullptr ? __builtin_strlen((const char *)s) : 0) {}
        
        strref(std::string const& s) : data(s.data()), len(s.size()) {}
        strref(std::string&&) = delete;
        constexpr strref(std::nullptr_t) : data(nullptr), len(0) {}
        constexpr strref(const char *str, size strlen) : data(str), len(strlen) {}
        constexpr strref(const byte *str, size strlen) : data(&reinterpret_cast<const char&>(*str)), len(strlen) {}
        template <size_t N> constexpr strref(const char (&str)[N]) : data(str), len(N-1){}
        template <size_t N> constexpr strref(const uint8 (&bytes)[N]) : data((char*)bytes), len(N) { }
        
        TempCStr tmp_c_str();
        
        constexpr strref slice(usize i) const {
            return assert(i <= len, exceptions::BadIndex, i, len),
                   strref(data+i, len-i);
        }
        constexpr strref slice(usize i, usize j) const {
            return assert(i <= len, exceptions::BadIndex, i , len),
                   assert(i <= j, exceptions::BadIndex, i, j),
                   strref(data+i, j-i);
        }
        
        constexpr char operator [] (usize i) const {
            return assert(i < len, exceptions::BadIndex, i, len-1),
                   data[i];
        }
        
        explicit operator bool () const {
            return data != nullptr;
        }
        operator std::string () const {
            return std::string(data, len);
        }
        constexpr strref operator() (usize i, usize j) const {
            return slice(i, j);
        }
        constexpr strref operator() (usize i) const {
            return slice(i);
        }
    };

    constexpr const char *begin(strref s)       { return s.data; }
    constexpr const char *end(strref s)         { return s.data+s.len; }
    constexpr size        len(strref s)         { return s.len; }
    std::ostream&  operator << (std::ostream & o, const strref str);
    constexpr bool operator == (strref left, strref right) {
        return (left.len != right.len) ?
            false :
                left.data == right.data ?
                    true : memcmp(left.data, right.data, left.len) == 0;
    }
    constexpr bool operator != (strref left, strref right) {
        return !(left == right);
    }
    constexpr bool operator == (strref left, std::nullptr_t) {
        return left.data == nullptr;
    }
    constexpr bool operator == (std::nullptr_t, strref right) {
        return right.data == nullptr;
    }
    
    constexpr bool operator != (strref left, std::nullptr_t) {
        return left.data != nullptr;
    }
    constexpr bool operator != (std::nullptr_t, strref right) {
        return right.data != nullptr;
    }

    struct bufref {
        byte * data;
        size   len;
        
        constexpr bufref()                     : data(0), len(0) {}
        constexpr bufref(char *data, usize len) : data(&reinterpret_cast<byte&>(*data)), len(len) {}
        constexpr bufref(byte *data, usize len) : data(data), len(len) {}
        template <size N> constexpr bufref(char (&str)[N]) : data((byte*)str), len(N) {}
        template <size N> constexpr bufref(uint8 (&bytes)[N]) : data(bytes), len(N) {}
        
        bufref slice(usize i) const { 
            return assert(i <= len, exceptions::BadIndex, i, len), 
                   bufref(data+i, len-i); 
        }
        
        bufref slice(usize i, usize j) const  { 
            return assert(i <= len, exceptions::BadIndex, i , len),
                   assert(i <= j, exceptions::BadIndex, i, j),
                   bufref(data+i, j-i); 
        }
        
        constexpr byte& operator [] (usize i) const {
            return assert(i < len, exceptions::BadIndex, i,len-1), 
                   data[i];
        }
        
//         constexpr byte operator [] (size i) const {
//             return assert(i < len), data[i];
//         }
        
        bufref operator() (usize i, usize j) const {
            return slice(i, j);
        }
        
        bufref operator() (usize i) const  {
            return slice(i);
        }
        
        explicit operator bool () const {
            return data != nullptr;
        }
        explicit operator std::string () const {
            return std::string((char*)data, len);
        }
        
        constexpr operator strref () const {
            return strref(data, len);
        }
        
        bufref& operator ++ () {
            assert(len > 0, exceptions::BadIndex);
            len--;
            data++;
            return *this;
        }
        
        bufref operator ++ (int) {
            assert(len > 0, exceptions::BadIndex);
            bufref copy = *this;
            len--;
            data++;
            return copy;
            
        }
    };

    // constexpr bool operator == (str left, str right) {
    //     if (left.len != right.len)
    //         return false;
    //     if (left.data == right.data)
    //         return true;
    //     
    //     return memcmp(left.data, right.data, left.len) == 0;
    // }

    constexpr byte *begin(bufref b)             { return b.data; }
    constexpr byte *end(bufref b)               { return (b.data+b.len); }
    constexpr size  len(bufref b)               { return b.len; }

    inline size copy(bufref dst, strref src) {
        size n = src.len > dst.len ? dst.len : src.len;
        memmove(dst.data, src.data, n);
        return n;
    }


    
    template <usize N>
    bufref slice(char (&arr)[N], usize i, usize j=N) {
        return assert(i <= j && j <= N , exceptions::BadIndex, std::min(j, N)), 
               bufref(arr+i, j-i);
    }
    
    template <usize N>
    bufref slice(byte (&arr)[N], usize i, usize j=N) {
        return assert(i <= j, exceptions::BadIndex, i, j),
               assert(j <= N, exceptions::BadIndex, j, N),
               bufref(arr+i, j-i);
    }
    
    template <usize N>
    strref slice(const char (&arr)[N], usize i, usize j=N) {
        return assert(i <= j, exceptions::BadIndex, i, j), 
               assert(j <= N, exceptions::BadIndex, j, N),
               strref(arr+i, j-i);
    }
    
    template <usize N>
    strref slice(const byte (&arr)[N], usize i, usize j=N) {
        return assert(i <= j, exceptions::BadIndex, i, j), 
               assert(j <= N, exceptions::BadIndex, j, N),
               strreff(arr+i, j-i);
    }

    //template <size N> 
    //using SmallStr   = SmallBuf<N>;

    #include "str.inlines.h"
}

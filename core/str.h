#pragma once
#include <string>
#include <string.h>
#include <stddef.h>

#include <axe/core/typedefs.h>
#include <axe/core/assert.h>
#include <axe/core/alloc.h>

namespace axe {
    using str        = struct strref;
    using ByteBuf    = struct Buf;
    using StrBuf     = struct Buf;
    using Str        = struct Buf;

    using byteref    = struct bufref;
    using buf        = struct bufref;

    struct bufref {
        byte * data;
        size   len;
        
        constexpr bufref();
        constexpr bufref(char *data, size len);
        constexpr bufref(byte *data, size len);
        
        bufref slice(size i) const;
        bufref slice(size i, size j) const;
        byte& operator [] (size i) const;
        bufref operator() (size i, size j) const;
        bufref operator() (size i) const;
        explicit operator bool () {
            return data != nullptr;
        }
        explicit operator std::string () {
            return std::string((char*)data, len);
        }
        constexpr operator strref const ();
    };

    // constexpr bool operator == (str left, str right) {
    //     if (left.len != right.len)
    //         return false;
    //     if (left.data == right.data)
    //         return true;
    //     
    //     return memcmp(left.data, right.data, left.len) == 0;
    // }

    constexpr char *begin(bufref b);
    constexpr char *end(bufref b);
    constexpr size  len(bufref b);


    struct TempCStr;
    
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

    struct strref {
        const char * data;
        size         len;
        
        constexpr strref();
        constexpr strref(const char *s) : data(s), len(__builtin_strlen(s)) {}
        explicit strref(std::string const& s) : data(s.data()), len(s.size()) {}
        //constexpr strref(const bufref b);
        constexpr strref(std::nullptr_t);
        constexpr strref(const char *str, size strlen);
        template <size N> constexpr strref(const char (&str)[N]);
        template <size N> constexpr strref(const uint8 (&bytes)[N]);
        
        TempCStr tmp_c_str();
        
        constexpr strref slice(size i) const;
        constexpr strref slice(size i, size j) const;
        
        constexpr char operator [] (size i) const;
        explicit operator bool ();
        explicit operator std::string () {
            return std::string(data, len);
        }
        constexpr strref operator() (size i, size j) const;
        constexpr strref operator() (size i) const;
    };

    constexpr const char *begin(strref s);
    constexpr const char *end(strref s);
    constexpr size        len(strref s);
    std::ostream&  operator << (std::ostream & o, const strref str);
    constexpr bool operator == (strref left, strref right);
    constexpr bool operator != (strref left, strref right);
    constexpr bool operator == (strref left, std::nullptr_t);
    constexpr bool operator == (std::nullptr_t, strref right);

    size copy(bufref dst, strref src);

    template <typename ArgFirst, typename ArgSecond, typename... ArgRest>
    size len(ArgFirst a1, ArgSecond a2, ArgRest... args) {
        return len(a1) + len(a2, args...);
    }

    struct Buf {
        bufref     buf;
        size       len;
        Allocator& alloc;
        
        Buf(Allocator& alloc, byte *buf, size capacity);
        Buf(Allocator& alloc, size capacity=0);
        
        void append(str s);
        void append(char c);
        void append(byte b);
        void append(rune r);
        
        void append_many(str s, size count);
        void append_many(byte b, size count);
        void append_many(char c, size count);
        void append_many(rune r, size count);
        
        
        void ensure(size cap);
        
        template <typename ArgFirst, typename ArgSecond, typename... ArgRest>
        void append(const ArgFirst &, const ArgSecond &, const ArgRest &...);
        
        operator bufref ();
        operator strref ();
        
        template <typename T>
        Buf& operator += (T const& s);
        
        explicit operator std::string () {
            return std::string((char*) buf.data, len);
        }
        
        Buf(Buf     const& other) = delete;
        Buf& operator = (Buf const& other) = delete;
    };

    constexpr size len(Buf const& buf) {
        return buf.len;
    }

    template <size N>
    struct SmallBuf : Buf {
        byte fixedbuf[N];
        SmallBuf(Allocator&);
    } ;

    struct TempCStr {
        char * charp;
        char buf[256];
        
        TempCStr(const char *data, size len);
        operator const char * ();
        ~TempCStr();
    } ;

    template <size N> 
    using SmallStr   = SmallBuf<N>;

    #include "str.inlines.h"
}

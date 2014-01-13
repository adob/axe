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

    using byteref    = struct bufref;
    using buf        = struct bufref;
    using String     = struct Buffer;
    
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
        
        constexpr strref() : data(nullptr), len(0) {}
        constexpr strref(const char *s) : data(s), len(s != nullptr ? __builtin_strlen(s) : 0) {}
        explicit strref(std::string const& s) : data(s.data()), len(s.size()) {}
        constexpr strref(std::nullptr_t) : data(nullptr), len(0) {}
        constexpr strref(const char *str, size strlen) : data(str), len(strlen) {}
        constexpr strref(const byte *str, size strlen) : data((char*)str), len(strlen) {}
        template <size N> constexpr strref(const char (&str)[N]) : data(str), len(N-1){}
        template <size N> constexpr strref(const uint8 (&bytes)[N]) : data((char*)bytes), len(N) { }
        
        TempCStr tmp_c_str();
        
        constexpr strref slice(size i) const {
            return assert(i <= len), 
            strref(data+i, len-i);
        }
        constexpr strref slice(size i, size j) const {
            return assert(i <= len && i <= j),
            strref(data+i, j-i);
        }
        
        constexpr char operator [] (size i) const {
            return assert(i < len), data[i];
        }
        
        explicit operator bool () const {
            return data != nullptr;
        }
        explicit operator std::string () const {
            return std::string(data, len);
        }
        constexpr strref operator() (size i, size j) const {
            return slice(i, j);
        }
        constexpr strref operator() (size i) const {
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

    struct bufref {
        byte * data;
        size   len;
        
        constexpr bufref()                     : data(0), len(0) {}
        constexpr bufref(char *data, size len) : data((byte*)data), len(len) {}
        constexpr bufref(byte *data, size len) : data(data), len(len) {}
        template <size N> constexpr bufref(char (&str)[N]) : data((byte*)str), len(N) {}
        template <size N> constexpr bufref(uint8 (&bytes)[N]) : data(bytes), len(N) {}
        
        bufref slice(size i) const { 
            assert(i <= len); 
            return bufref(data+i, len-i); 
        }
        
        bufref slice(size i, size j) const  { 
            assert(j <= len && i <= j); 
            return bufref(data+i, j-i); 
        }
        
        constexpr byte& operator [] (size i) const {
            return assert(i < len), data[i];
        }
        
//         constexpr byte operator [] (size i) const {
//             return assert(i < len), data[i];
//         }
        
        bufref operator() (size i, size j) const {
            return slice(i, j);
        }
        
        bufref operator() (size i) const  {
            return slice(i);
        }
        
        explicit operator bool () const {
            return data != nullptr;
        }
        explicit operator std::string () const {
            return std::string((char*)data, len);
        }
        
        constexpr operator strref () const {
            return strref((char*)data, len);
        }
        
        bufref& operator ++ () {
            assert(len > 0);
            len--;
            data++;
            return *this;
        }
        
        bufref operator ++ (int) {
            assert(len > 0);
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

    constexpr char *begin(bufref b)             { return (char*) b.data; }
    constexpr char *end(bufref b)               { return (char*) (b.data+b.len); }
    constexpr size  len(bufref b)               { return b.len; }

    inline size copy(bufref dst, strref src) {
        size n = src.len > dst.len ? dst.len : src.len;
        memmove(dst.data, src.data, n);
        return n;
    }

    template <typename ArgFirst, typename ArgSecond, typename... ArgRest>
    size len(ArgFirst a1, ArgSecond a2, ArgRest... args) {
        return len(a1) + len(a2, args...);
    }

    struct Str {
        bufref     buf;
        size       len;
        Allocator& alloc;
        
        Str(Allocator& alloc, byte *buf, size capacity)  : buf(buf, capacity), len(0), alloc(alloc) {}
        Str(Allocator& alloc, size capacity=0): buf(alloc(capacity)) , len(0), alloc(alloc) {}
        
        void append(str s) {
            ensure(s.len);
            memmove(buf.data + len, s.data, s.len);
            len += s.len;
        }
        void append(char c) {
            ensure(1);
            buf[len] = c;
            len += 1;
        }
        void append(byte b) {
            append((char) b);
        }
        void append(rune r);
        
        void append_many(str s, size count)  {
            ensure(s.len * count);
            for (size i = 0; i < count; i++) {
                memmove(buf.data + len, s.data, s.len);
                len += s.len;
            }
        }
        void append_many(byte b, size count) {
            append_many((char) b, count);
        }
        void append_many(char c, size count) {
            ensure(count);
            memset(buf.data + len, c, count);
            len += count;
        }
        void append_many(rune r, size count);
        
        
        void ensure(size cap) {
            if (len + cap > buf.len) {
                bufref newbuf = alloc(std::max(cap+buf.len, buf.len * 2));
                copy(newbuf, buf);
                buf = newbuf;
                
            }
        }
        
        
        template <typename ArgFirst, typename ArgSecond, typename... ArgRest>
        void append(const ArgFirst& a1, const ArgSecond& a2, const ArgRest &... args)  {
            append(a1);
            append(a2, args...);
        }
        
        operator bufref () {
            return buf(0, len);
        }
        operator strref () {
            return buf(0, len);
        }
        
        template <typename T>
        Str& operator += (T const& s) {
            append(s);
            return *this;
        }
        
        explicit operator std::string () {
            return std::string((char*) buf.data, len);
        }
        
        Str(Str     const& other) = delete;
        Str& operator = (Str const& other) = delete;
    };
    
    struct Buffer {
        byte *data;
        size len;
        
        Buffer() : data(nullptr), len(0) {}
        explicit Buffer(size len) : data((byte*)malloc(len)), len(len) {}
        Buffer(Buffer&& other) : data(other.data), len(other.len) {
            other.data = nullptr;
        }
        
        void ensure(size newsize) {
            if (newsize > len) {
                len = std::min(newsize, len*2);
                data = (byte*) realloc(data, len);
            }
        }
        
        Buffer& operator = (Buffer&& other) {
            if (data) {
                free(data);
            }
            data = other.data;
            len  = other.len;
            other.data = nullptr;
            return *this;
        }
        
        bufref slice(size i, size j) const  { 
            assert(j <= len && i <= j); 
            return bufref(data+i, j-i); 
        }
        
        bufref slice(size i) const { 
            assert(i <= len); 
            return bufref(data+i, len-i); 
        }
        
        bufref operator() (size i, size j) const {
            return slice(i, j);
        }
        
        bufref operator() (size i) const  {
            return slice(i);
        }
        
        byte& operator [] (size i) {
            assert(i < len);
            return data[i];
        }
        
        byte operator [] (size i) const {
            assert(i < len);
            return data[i];
        }
        
        operator bufref () const {
            return bufref(data, len);
        }
        
        operator strref () const {
            return strref(data, len);
        }
        
        ~Buffer() {
            if (data) {
                free(data);
            }
        }
    } ;
    
    constexpr size len(Buffer const& buffer) {
        return buffer.len;
    }

    constexpr size len(Str const& buf) {
        return buf.len;
    }

    template <size N>
    struct SmallStr : Str {
        byte fixedbuf[N];
        SmallStr(Allocator&) : Str(alloc, fixedbuf, N) {}
    } ;

    struct TempCStr {
        char * charp;
        char buf[256];
        
        TempCStr(const char *data, size len);
        operator const char * ();
        ~TempCStr();
    } ;

    //template <size N> 
    //using SmallStr   = SmallBuf<N>;

    #include "str.inlines.h"
}

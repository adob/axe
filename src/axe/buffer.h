#import "str.h"
#import "alloc.h"
namespace axe {
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
        
        //         void append(const byte *s, size nbytes) {
            //             ensure(nbytes);
        //             memcpy(buf.data + len, s, nbytes);
        //             len += nbytes;
        //         }
        
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
        
        
        void ensure(usize cap) {
            if (len + cap > buf.len) {
                bufref newbuf = alloc(std::max((size)cap+buf.len, buf.len * 2));
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
        size cap;
        
        Buffer() : data(nullptr), len(0), cap(0) {}
        Buffer(size len, size cap) : data((byte*)malloc(cap)), len(len), cap(cap) {}
        Buffer(size len) : data((byte*)malloc(len)), len(len), cap(len) {}
        Buffer(Buffer&& other) : data(other.data), len(other.len), cap(other.cap) {
            other.data = nullptr;
        }
        
        Buffer(str s) {
            data = (byte *) malloc(s.len);
            len  = s.len;
            cap  = s.len;
            memmove(data, s.data, len);
        }
        
        Buffer(const char *strp) {
            size slen = strlen(strp);
            data = (byte *) malloc(slen);
            len  = slen;
            cap  = slen;
            memmove(data, strp, len);
        }
        
        template <size N>
        Buffer(const char (&arr)[N]) : data((byte*)malloc(N-1)), len(N-1), cap(N-1) {
            memmove(data, arr, N-1);
        }
        
        void ensure(usize newsize) {
            if (newsize > cap) {
                cap = std::max((size)newsize, std::max(cap*2, (size)16));
                data = (byte*) realloc(data, cap);
            }
        }
        
        void append(str s) {
            ensure(len + s.len);
            memmove(data + len, s.data, s.len);
            len += s.len;
        }
        
        Buffer& operator += (str s) {
            append(s);
            return *this;
        }
        
        size write(str s) {
            append(s);
            return axe::len(s);
        }
        
        void append(char c) {
            ensure(len + 1);
            data[len] = c;
            len += 1;
        }
        Buffer& operator += (char c) {
            append(c);
            return *this;
        }
        void append(byte b) {
            append((char) b);
        }
        Buffer& operator += (byte b) {
            append(b);
            return *this;
        }
        void append(rune r);
        Buffer& operator += (rune r) {
            append(r);
            return *this;
        }
        
        void append_many(str s, usize count)  {
            ensure(len + s.len * count);
            for (size i = 0; i < count; i++) {
                memmove(data + len, s.data, s.len);
                len += s.len;
            }
        }
        void append_many(byte b, usize count) {
            append_many((char) b, count);
        }
        void append_many(char c, usize count) {
            ensure(len + count);
            memset(data + len, c, count);
            len += count;
        }
        void append_many(rune r, usize count);
        
        void clear() {
            len = 0;
        }
        
        str string(Allocator& alloc) const {
            Str s(alloc);
            s.append(str(data, len));
            return s;
        }
        
        Buffer& operator = (const char *strp) {
            len = strlen(strp);
            ensure(len);
            
            memmove(data, strp, len);
            return *this;
        }
        
        Buffer& operator = (str s) {
            ensure(s.len);
            
            memmove(data, s.data, s.len);
            len = s.len;
            return *this;
        }
        
        Buffer& operator = (Buffer&& other) {
            if (data) {
                free(data);
            }
            data = other.data;
            len  = other.len;
            cap  = other.cap;
            other.data = nullptr;
            return *this;
        }
        
        bufref slice(usize i, usize j) const  { 
            assert(j <= len && i <= j, exceptions::BadIndex, std::min(i, j));
            return bufref(data+i, j-i); 
        }
        
        bufref slice(usize i) const { 
            assert(i <= len, exceptions::BadIndex, len); 
            return bufref(data+i, len-i); 
        }
        
        bufref operator() (usize i, usize j) const {
            return slice(i, j);
        }
        
        bufref operator() (usize i) const  {
            return slice(i);
        }
        
        byte& operator [] (usize i) {
            assert(i < len, exceptions::BadIndex, len-1);
            return data[i];
        }
        
        byte operator [] (usize i) const {
            assert(i < len, exceptions::BadIndex, len-1);
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
}
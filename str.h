#pragma once
#include <string.h>
#include <stddef.h>

#include "typedefs.h"
#include "assert.h"

struct bufref {
    char * data;
    size   len;
    
    constexpr bufref() : data(0), len(0) {}
    constexpr bufref(char *data, size len) : data(data), len(len) {}
    
    bufref slice(size i) const { assert(i < len); return bufref(data+i, len-i); }
    bufref slice(size i, size j) const { 
        assert(i < len && j <= len && i <= j); 
        return bufref(data+i, j-i); 
    }
};

constexpr char * begin(bufref b)             { return b.data; }
constexpr char * end(bufref b)               { return b.data+b.len; }
constexpr size len(bufref b)                 { return b.len; }


struct strref {
    const char * data;
    size         len;
    
    constexpr strref() : data(nullptr), len(0) {}
    /*constexpr*/ strref(const char *str) : data(str), len(strlen(str)) { }
    constexpr strref(const bufref b) : data(b.data), len(b.len) {}
    
    constexpr strref(nullptr_t) : data(nullptr), len(0) {}
    
    constexpr strref(const char *str, size strlen) : data(str), len(strlen) {}
    
    template <size N>
    constexpr strref(const char (&str)[N]) : data(str), len(N-1){}
    
    template <size N>
    constexpr strref(const uint8 (&bytes)[N]) : data((char*)bytes), len(N) { }
    
//     constexpr
//     size size() const {
//         return len;
//     }
//  
    constexpr strref slice(size i) const {
        return assert(i <= len), 
        strref(data+i, len-i);
    }
    constexpr strref slice(size i, size j) const {
        return assert(i <= len && i <= j),
               strref(data+i, j-i);
    }
    
    constexpr char operator [] (size i) const {
        return assert(i < len),
               data[i];
    }
    
    operator bool () {
        return len == 0;
    }
    
    constexpr strref operator()(size i, size j) {
        return slice(i, j);
    }
    constexpr strref operator()(size i) {
        return slice(i);
    }
};

constexpr const char * begin(strref s)       { return s.data; }
constexpr const char * end(strref s)         { return s.data+s.len; }
constexpr size len(strref s)                 { return s.len; }


std::ostream& operator << (std::ostream & o, const strref str);

constexpr bool operator == (strref left, strref right) {
    return (left.len != right.len) ?
        /*then */ false :
        /*else */ memcmp(left.data, right.data, left.len) == 0;
}


constexpr bool operator == (strref left, nullptr_t) {
    return left.data == nullptr;
}
constexpr bool operator == (nullptr_t, strref right) {
    return right.data == nullptr;
}

// constexpr
// bool operator == (strref left, strref right) {
//     return (left.len != right.len) ?
//     /*then */ false :
//     /*else */ memcmp(left.data, right.data, left.len) == 0;
// }


struct Buf {
    char  *data;
    size   len;

    Buf() : data(0), len(0) { }
    
    Buf(size len) : data((char*)malloc(len)), len(len) {}
    
    Buf(Buf const& other) = default;
    
    Buf(Buf && other) : data(other.data), len(other.len) { 
        other.data = nullptr;
        other.len = 0;
    }
    
    Buf(strref str)  {
        initcopy(str.data, str.len);
    }
    
    Buf& operator = (Buf const& other) {
        copy(other.data, other.len);
        return *this;
    }
    
    Buf& operator = (Buf&& other) {
         data = other.data;
         len  = other.len;
         other.data = nullptr;
         other.len = 0;
         return *this;
     }
    
    Buf& operator = (strref other) {
        copy(other.data, other.len);
        return *this;
    }
    
    operator bufref () const {
        return bufref(data, len);
    }
    
    bufref slice(size i, size j) const {
        assert(i <= j && j <= len);
        return bufref(data+i, j-i);
    }
    bufref slice(size i) const {
        assert(i <= len);
        return bufref(data+i, len-i);
    }
    
    bufref operator()(size i, size j) const {
        return slice(i, j);
    }
    bufref operator()(size i) const {
        return slice(i);
    }
    
    bufref append(strref data);
    bufref append(char c);
    
    void clear() {
        len = 0;
    }
    
    void reset() {
        free(data);
        data = 0;
        len  = 0;
    }
    
    ~Buf() {
        if (data)
            free(data);
    }
    
    void initcopy(const char *data, size len) {
        this->data = (char*) malloc(len);
        assert(this->data != 0);
        memcpy(this->data, data, len);
        this->len = len;
    }
    
//     void initmove(char *data, size len) {
//         this->data = data;
//         this->len = len;
//     }
    
    void move(char *data, size len) {
        this->data = data;
        this->len  = len;
    }
    
    void copy(const char *otherdata, size otherlen) {
        if (otherlen > this->len) {
            this->data = (char *) realloc(this->data, otherlen);
            assert(this->data != 0);
        }
        memcpy(this->data, otherdata, otherlen);
        this->len = otherlen;
    }
};


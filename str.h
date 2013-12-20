#pragma once
#include <string.h>

#include "typedefs.h"
#include "assert.h"

struct strref {
    const char * const data;
    const size         len;
    
    //constexpr 
    strref(const char *str) : data(str), len(strlen(str)) { }
    
    constexpr 
    strref(const char *str, size strlen) : data(str), len(strlen) {}
    
    template <size N>
    constexpr
    strref(const char (&str)[N]) : data(str), len(N-1){}
    
    template <size N>
    constexpr strref(const uint8 (&bytes)[N]) : data((char*)bytes), len(N) { }
    
//     constexpr
//     size size() const {
//         return len;
//     }
//     
    constexpr strref slice(size i, size j) const {
        return assert(i < len && j <= len && i <= j),
               strref(data+i, j-i);
    }
    
    constexpr strref slice(size i) const {
        return assert(i < len), 
               strref(data+i, len-i);
    }
    
    constexpr char operator [] (size i) const {
        return assert(i < len),
               data[i];
    }
};

std::ostream& operator << (std::ostream & o, const strref str);

constexpr
bool operator == (strref left, strref right) {
    return (left.len != right.len) ?
        /*then */ false :
        /*else */ memcmp(left.data, right.data, left.len) == 0;
}

struct bufref {
    char * const  data;
    const size    len;
    
    bufref() : data(0), len(0) {}
    bufref(char *data, size len) : data(data), len(len) {}
};

struct buf {
    char  *data;
    size   len;
    
    buf() : data(0), len(0) { }
    
    buf(size len) : data((char*)malloc(len)), len(len) {}
    
    buf(buf const& other) = default;
//     {
//         initcopy(other.data, other.len);
//     }
    
    buf(buf && other) = default;
//    : data(other.data), len(other.len) { }
    
    buf(strref str)  {
        initcopy(str.data, str.len);
    }
    
    buf& operator = (buf const& other) {
        copy(other.data, other.len);
        return *this;
    }
    
    buf& operator = (buf&& other) = default;
//     {
//         move(other.data, other.len);
//         return *this;
//     }
    
    buf& operator = (strref other) {
        copy(other.data, other.len);
        return *this;
    }
    
    bufref slice(size from, size to) {
        assert(from <= to && from < len && to <= len);
        return bufref(data, to-from);
    }
    
    ~buf() {
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


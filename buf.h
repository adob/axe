#pragma once
#include <cstdlib>
#include "datatypes.h"
#include "str.h"
#include "assert.h"
#include "slice.h"

#include "print.h"

namespace alex {

struct buf {
    char  *data;
    size   len;
};

struct strbuf {
    char  *data;
    size   len;
    
    strbuf() : data(0), len(0) { }
    
    strbuf(strbuf const& other) {
        initcopy(other.data, other.len);
    }
    
    strbuf(strbuf && other) : data(other.data), len(other.len) { }
    
    strbuf(str str)  {
        initcopy(str.data, str.len);
    }
    
    strbuf& operator = (strbuf const& other) {
        copy(other.data, other.len);
        return *this;
    }
    
    strbuf& operator = (strbuf&& other) {
        move(other.data, other.len);
        return *this;
    }
    
    strbuf& operator = (str other) {
        copy(other.data, other.len);
        return *this;
    }
    
    alex::slice<char> slice(size from, size to) {
        assert(from <= to && from < len && to <= len);
        return alex::slice<char>(data, to-from);
    }
    
    ~strbuf() {
        std::free(data);
    }
    
    void initcopy(const char *data, size len) {
        this->data = (char*) std::malloc(len);
        assert(this->data != 0);
        std::memcpy(this->data, data, len);
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
            this->data = (char *) std::realloc(this->data, otherlen);
            assert(this->data != 0);
        }
        std::memcpy(this->data, otherdata, otherlen);
        this->len = otherlen;
    }
};
}
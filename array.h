#pragma once
#include <cstdlib>
#include "datatypes.h"
#include "str.h"
#include "assert.h"
#include "slice.h"

#include "print.h"

template <size N>
struct fixedbuf {
    char  data[N];
    
    fixedbuf() { }
    fixedbuf(char c) {
        memset(data, c, N);
    }
    
//     strbuf(strbuf const& other) {
//         initcopy(other.data, other.len);
//     }
//     
//     strbuf(strbuf && other) : data(other.data), len(other.len) { }
//     
//     strbuf(str str)  {
//         initcopy(str.data, str.len);
//     }
//     
//     strbuf& operator = (strbuf const& other) {
//         copy(other.data, other.len);
//         return *this;
//     }
//     
//     strbuf& operator = (strbuf&& other) {
//         move(other.data, other.len);
//         return *this;
//     }
//     
//     strbuf& operator = (str other) {
//         copy(other.data, other.len);
//         return *this;
//     }

    operator alex::slice<char> () { return alex::slice<char>(data, N); }
    
    alex::slice<char> slice(size i, size j) {
        assert(i <= j && i < N && j <= N);
        return alex::slice<char>(data+i, j-i);
    }
    
};

#pragma once
#include <cstring>

#include "datatypes.h"
#include "assert.h"
namespace alex {
struct str {
    const char * const data;
    const size         len;
    
    //constexpr 
    str(const char *str) 
        : data(str)
        , len(strlen(str))
    {
        
    }
    
    constexpr 
    str(const char *str, size strlen) 
        : data(str)
        , len(strlen)
    {
        
    }
    
    template <size N>
    constexpr
    str(const char (&str)[N]) 
        : data(str)
        , len(N-1)
    {
        
    }
    
    template <size N>
    constexpr
    str(const uint8 (&bytes)[N]) 
        : data((char*)bytes)
        , len(N)
    {
        
    }
    
//     constexpr
//     alex::size size() const {
//         return len;
//     }
//     
    constexpr
    str slice(size i, size j) const {
        return assert(i < len && j <= len && i <= j),
               str(data+i, j-i);
    }
    
    constexpr
    str slice(alex::size i) const {
        return assert(i < len), 
               str(data+i, len-i);
    }
    
    constexpr
    char operator [] (alex::size i) const {
        return assert(i < len),
               data[i];
    }
};


std::ostream& operator << (std::ostream & o, const str str);

constexpr
bool operator == (str left, str right) {
    return (left.len != right.len) ?
        /*then */ false :
        /*else */ std::memcmp(left.data, right.data, left.len) == 0;
}

}
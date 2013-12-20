#pragma once

namespace alex {
    
template <typename T>
struct slice {
    T * const  data;
    const size len;
    
    constexpr
    slice(T *data, size len)
        : data(data)
        , len(len)
    {
        
    }
        
};
    
}
#pragma once
#include <string>

#include <axe/core.h>
#include <axe/io.h>
#include <axe/unicode/utf8.h>

namespace axe {
namespace fmt {
    
    
    template <typename... Args>
    void printf(str format, Args... args);
    
    template <typename... Args>
    str sprintf(Allocator&, str format, Args... args);
    
    template <typename... Args>
    std::string sprintf(str format, Args... args);
    
    template <typename T>
    str stringify(T const& t, Allocator& alloc);
    
    template <typename T>
    std::string stringify(T const& t);
    
    #include <axe/fmt/fmt.inlines.h>
}
}


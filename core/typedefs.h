#include <stddef.h>
#include <stdint.h>

namespace axe {
    
    using size    = size_t;
    using byte    = uint8_t;
    using rune    = char32_t;
    using char16  = char16_t;
    using char32  = char32_t;
    using int8    = int8_t;
    using int32   = int32_t;
    using int64   = int64_t;
    using int128  = __int128;
    using uint8   = uint8_t;
    using uint16  = uint16_t;
    using uint32  = uint32_t;
    using uint64  = uint64_t;
    using uint128 = unsigned __int128;
    using float32 = float;
    using float64 = double;
    using intptr  = intptr_t;
    using uintptr = uintptr_t; 

    const size npos = (size) -1;
    const decltype(nullptr) nil = nullptr;
    
}

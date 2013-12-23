#pragma once
#include "builtin.h"

namespace strconv {
    
    // Quote returns a double-quoted Go string literal representing s.  The
    // returned string uses Go escape sequences (\t, \n, \xFF, \u0100) for
    // control characters and non-printable characters as defined by
    // IsPrint.
    // str quote(str s, Allocator&);
}
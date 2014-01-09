#pragma once 
#include "core.h"
namespace axe {
namespace strconv { 
    // Quote returns a double-quoted Go string literal representing s.  The
    // returned string uses Go escape sequences (\t, \n, \xFF, \u0100) for
    // control characters and non-printable characters as defined by
    // IsPrint.
    str quote(str s, Allocator& alloc);
    
    // QuoteToASCII returns a double-quoted Go string literal representing s.
    // The returned string uses Go escape sequences (\t, \n, \xFF, \u0100) for
    // non-ASCII characters and non-printable characters as defined by IsPrint.
    str quote_to_ascii(str s, Allocator& alloc);
    
    // QuoteRune returns a single-quoted Go character literal representing the
    // rune.  The returned string uses Go escape sequences (\t, \n, \xFF, \u0100)
    // for control characters and non-printable characters as defined by IsPrint.
    str quote_rune(rune r, Allocator& alloc);
    
    // QuoteRuneToASCII returns a single-quoted Go character literal representing
    // the rune.  The returned string uses Go escape sequences (\t, \n, \xFF,
    // \u0100) for non-ASCII characters and non-printable characters as defined
    // by IsPrint.
    str quote_rune_to_ascii(rune r, Allocator& alloc);
    
    // IsPrint reports whether the rune is defined as printable by Go, with
    // the same definition as unicode.IsPrint: letters, numbers, punctuation,
    // symbols and ASCII space.
    bool is_print(rune r);
}
}
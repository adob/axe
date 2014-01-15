#import <axe/core.h>

namespace axe { namespace unicode {
    const rune MaxRune = 0x0010FFFF;
    const rune ReplacementChar = 0xFFFD;
    const rune MaxASCII  = 0x7F;
    const rune MaxLatin1 = 0xFF;
    
    struct Range16 {
        uint16 lo;
        uint16 hi;
        uint16 stride;
    } ;
    
    struct Range32 {
        uint32 lo;
        uint32 hi;
        uint32 stride;
    } ;
    

    struct RangeTable {
        const array<Range16> r16; 
        const array<Range32> r32;
        int            latin_offset;
    } ;
    
    extern const RangeTable White_Space;
    
    // IsSpace reports whether the rune is a space character as defined
    // by Unicode's White Space property; in the Latin-1 space
    // this is
    //      '\t', '\n', '\v', '\f', '\r', ' ', U+0085 (NEL), U+00A0 (NBSP).
    // Other definitions of spacing characters are set by category
    // Z and property Pattern_White_Space.
    bool is_space(rune r);

}}
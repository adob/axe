#import <axe/core.h>

namespace axe { namespace utf8 {
    
    const rune RuneError = 0xFFFD;     // the "error" Rune or "Unicode replacement character"
    const rune RuneSelf  = 0x80;       // characters below Runeself are represented as themselves in a single byte.
    const rune MaxRune   = 0x0010FFFF; // Maximum valid Unicode code point.
    const rune UTFMax    = 4;          // maximum number of bytes of a UTF-8 encoded Unicode character. 
    
    // DecodeRuneInString is like DecodeRune but its input is a string.
    // If the encoding is invalid, it returns (RuneError, 1), an impossible result for correct UTF-8.
    // An encoding is invalid if it is incorrect UTF-8, encodes a rune that is
    // out of range, or is not the shortest possible UTF-8 encoding for the
    // value. No other validation is performed.
    rune decode(str s, int& nbytes);
    str encode(rune r, bufref b);
    str encode(rune r, Allocator& alloc);
    
    size count(str s);
    
    struct runestr {
        str s;
        
        struct RuneIterator {
            str          s;
            rune         r;
            
            void operator ++ ();
            rune operator * ();
            bool operator != (RuneIterator const&);
        } ;
        
        RuneIterator begin();
        RuneIterator end();
    } ; 
    
    runestr runes(str s);
    
}}
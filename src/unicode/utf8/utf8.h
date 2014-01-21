#import <axe/typedefs.h>

namespace axe {
    namespace unicode {
        
        /// <pre>
        ///   #include &lt;axe/unicode/utf8.h&gt;
        ///   use axe::unicode::utf8;
        ///   use axe::utf8;
        /// </pre>
        ///
        /// namespace utf8 implements functions and constants to support text encoded in UTF-8. 
        /// It includes functions to translate between runes and UTF-8 byte sequences.
        namespace utf8 {
            
            const rune RuneError = 0xFFFD;     ///< the "error" Rune or "Unicode replacement character"
            const rune RuneSelf  = 0x80;       ///< characters below Runeself are represented as themselves in a single byte.
            const rune MaxRune   = 0x0010FFFF; ///< maximum valid Unicode code point.
            const rune UTFMax    = 4;          ///< maximum number of bytes of a UTF-8 encoded Unicode character. 
            
            /// rune_start(c) reports wheather the byte c could be the first byte of an encoded rune.
            /// Second and subsequent bytes always have the top two bits set to 10.
            inline bool rune_start(char c) { return (c & 0xC0) != 0x80; }            
        }
    }
    
}
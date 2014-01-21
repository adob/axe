#import <axe/typedefs.h>
namespace axe {
    namespace unicode {
       
        // IsSpace reports whether the rune is a space character as defined
        // by Unicode's White Space property; in the Latin-1 space
        // this is
        //      '\t', '\n', '\v', '\f', '\r', ' ', U+0085 (NEL), U+00A0 (NBSP).
        // Other definitions of spacing characters are set by category
        // Z and property Pattern_White_Space.
        bool is_space(rune r);
       
    }
}
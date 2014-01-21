#import <axe/typedefs.h>
namespace axe { 
    namespace unicode {
        namespace utf8 {
            namespace internal {
                const rune SurrogateMin = 0xD800;
                const rune SurrogateMax = 0xDFFF;
                
                const byte T1 = 0x00; // 0000 0000
                const byte TX = 0x80; // 1000 0000
                const byte T2 = 0xC0; // 1100 0000
                const byte T3 = 0xE0; // 1110 0000
                const byte T4 = 0xF0; // 1111 0000
                const byte T5 = 0xF8; // 1111 1000
                
                const byte MaskX = 0x3F; // 0011 1111
                const byte Mask2 = 0x1F; // 0001 1111
                const byte Mask3 = 0x0F; // 0000 1111
                const byte Mask4 = 0x07; // 0000 0111
                
                const rune Rune1Max = (1<<7)  - 1;
                const rune Rune2Max = (1<<11) - 1;
                const rune Rune3Max = (1<<16) - 1;
            }
        }
    }
}
#import <axe/raise.h>
#import "utf8.h"
#import "codes.h"
#import "encode.h"

namespace axe {
    namespace unicode {
        namespace utf8 {
            using namespace utf8::internal;
            
            str encode(rune r, bufref b) {
                if (r <= Rune1Max) {
                    if (len(b) < 1)
                        raise("Buffer too small");
                    b[0] = r;
                    return b(0, 1);
                } else if (r <= Rune2Max) {
                    if (len(b) < 2)
                        raise("Buffer too small");
                    b[0] = T2 | (r >> 6);
                    b[1] = TX | (r & MaskX);
                    return b(0, 2);
                } else if (r > MaxRune || (SurrogateMin <= r && r <= SurrogateMax)) {
                    r = RuneError;
                } else if (r <= Rune3Max) {
                    if (len(b) < 3)
                        raise("Buffer too small");
                    b[0] = T3 | (r >> 12);
                    b[1] = TX | ((r >> 6) & MaskX);
                    b[2] = TX | (r & MaskX);
                    return b(0, 3);
                }
                
                if (len(b) < 4)
                    raise("Buffer too small");
                
                b[0] = T4 | (r >> 18);
                b[1] = TX | ((r >> 12) & MaskX);
                b[2] = TX | ((r >> 6) & MaskX);
                b[3] = TX | (r & MaskX);
                return b(0, 4);
            }
            
            str encode(rune r, Allocator& alloc) {
                bufref b = alloc(UTFMax);
                return encode(r, b);
            }
        }
    }
}
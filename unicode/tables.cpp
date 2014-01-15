#import "PKG.h"
namespace axe { namespace unicode {
    
const RangeTable White_Space = {
    (array<unicode::Range16>) (unicode::Range16[]) {
        {0x0009, 0x000d, 1},
        {0x0020, 0x0020, 1},
        {0x0085, 0x0085, 1},
        {0x00a0, 0x00a0, 1},
        {0x1680, 0x1680, 1},
        {0x180e, 0x180e, 1},
        {0x2000, 0x200a, 1},
        {0x2028, 0x2029, 1},
        {0x202f, 0x202f, 1},
        {0x205f, 0x205f, 1},
        {0x3000, 0x3000, 1}
    },
    {},
    4
} ;

}}
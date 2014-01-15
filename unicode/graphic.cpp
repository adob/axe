#import "PKG.h"

namespace axe { namespace unicode { namespace {
    //bool is_excluding_latin1
}

bool is_excluding_latin(RangeTable const& rangetab, rune r);

bool is_space(rune r) {
    if (r < MaxLatin1) {
        switch (r) {
            case '\t': case '\n': case '\v': case '\f': case '\r':
            case ' ': case 0x85: case 0xA0:
                return true;
        }
        return false;
    }
    return is_excluding_latin(White_Space, r);
}

}}

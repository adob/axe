#include "../utf8.h"

namespace axe {
namespace utf8 {

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

namespace internal {
    
rune decode_rune(str s, int& runesize, bool& isshort) {
    size n = len(s);
    rune r;
    if (n < 1) {
        return runesize=0, isshort=true, RuneError;
    }
    byte c0 = s[0];
    
    // 1-byte, 7-bit sequence
    if (c0 < TX) {
        return runesize=1, isshort=false, c0;
    }
    
    // unexpected continuation byte?
    if (c0 < T2) {
        return runesize=1, isshort=false, RuneError;
    }
    
    // need first continuation byte
    if (n < 3) {
        return runesize=1, isshort=true, RuneError;
    }
    byte c1 = s[1];
    if (c1 < TX || T2 <= c1) {
        return runesize=1, isshort=false, RuneError;
    }
    
    // 2-byte, 11-bit sequence
    if (c0 < T3) {
        r = ((c0 & Mask2) << 6) | (c1 & MaskX);
        if (r <= Rune1Max) {
            return runesize=1, isshort=false, RuneError;
        }
        return runesize=2, isshort=false, r;
    }
    
    // need a third continuation byte
    if (n < 3) {
        return runesize=1, isshort=true, RuneError;
    }
    byte c2 = s[2];
    if (c2 < TX || T2 <= c2) {
        return runesize=1, isshort=false, RuneError;
    }
    
    // 3-byte, 16-bit sequence?
    if (c0 < T4) {
        r = ((c0 & Mask3) << 12) | ((c1 & MaskX) << 6) | (c2 & MaskX);
        if (r <= Rune2Max) {
            return runesize=1, isshort=false, RuneError;
        }
        if (SurrogateMin <= r && r <= SurrogateMax) {
            return runesize=1, isshort=false, RuneError;
        }
        return runesize=3, isshort=false, r;
    }
    
    // need a third continuation byte
    if (n < 4) {
        return runesize=1, isshort=true, RuneError;
    }
    byte c3 = s[3];
    if (c3 < TX || T2 <= c3) {
        return runesize=1, isshort=true, RuneError;
    }
    
    // 4-byte, 21-bit sequence?
    if (c0 < T5) {
        r = ((c0 & Mask4) << 18) | ((c1 & MaskX) << 12) | ((c2 & MaskX) << 6) | (c3 & MaskX);
        if (r <= Rune3Max || MaxRune < r) {
            return runesize=1, isshort=true, RuneError;
        }
        return runesize=4, isshort=false, r;
    }
    
    // error
    return runesize=1, isshort=false, RuneError;
}

} // namespace internal

rune decode(str s, int& nbytes) {
    bool isshort;
    return internal::decode_rune(s, nbytes, isshort);
}

int encode(bufref b, rune r) {
    if (r <= Rune1Max) {
        b[0] = r;
        return 1;
    } else if (r <= Rune2Max) {
        b[0] = T2 | (r >> 6);
        b[1] = TX | (r & MaskX);
        return 2;
    } else if (r > MaxRune || (SurrogateMin <= r && r <= SurrogateMax)) {
        r = RuneError;
    } else if (r < Rune3Max) {
        b[0] = T3 | (r >> 12);
        b[1] = TX | ((r >> 6) & MaskX);
        b[2] = TX | (r & MaskX);
        return 3;
    }
    
    b[0] = T4 | (r >> 18);
    b[1] = TX | ((r >> 12) & MaskX);
    b[2] = TX | ((r >> 6) & MaskX);
    b[3] = TX | (r & MaskX);
    return 4;
}

str encode(rune r, Allocator& alloc) {
    bufref b = alloc(UTFMax);
    int nbytes = encode(b, r);
    return b(0, nbytes);
}


void runestr::RuneIterator::operator ++ () {
    int nbytes;
    r = decode(s, nbytes);
    s = s(nbytes);
}

rune runestr::RuneIterator::operator * () {
    return r;
}

bool runestr::RuneIterator::operator != (RuneIterator const&) {
    return len(s) != 0;
}

runestr::RuneIterator runestr::begin() {
    RuneIterator ri{s, 0};
    ++ri;
    return ri;
}
runestr::RuneIterator runestr::end() {
    return RuneIterator{nil, 0};
}

runestr runes(str s) {
    return runestr{s};
}

size count(str s) {
    size count = 0;
    for (rune r : runes(s)) {
        (void) r;
        count++;
    }
    return count;
}

} // namespace utf8
}
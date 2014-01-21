#import "utf8.h"
#import "codes.h"
#import "decode.h"

namespace axe {
    namespace unicode {
        namespace utf8 {
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
                    if (n < 2) {
                        return runesize=1, isshort=true, RuneError;
                    }
                    byte c1 = s[1];;
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
            
            rune decode(str s) {
                bool isshort;
                int nbytes;
                return internal::decode_rune(s, nbytes, isshort);
            }
            
            rune decode_last(str s, int& nbytes) {
                size end = len(s);
                if (end == 0) {
                    return nbytes=0, RuneError;
                }
                size start = end - 1;
                rune r = s[start];
                if (r < RuneSelf) {
                    return nbytes=1, r;
                }
                // guard against O(n^2) behavior when traversing
                // backwards through strings with long sequences of
                // invalid UTF-8.
                size lim = end - UTFMax;
                if (lim < 0) {
                    lim = 0;
                }
                for (start--; start >= lim; start--) {
                    if (rune_start(s[start])) {
                        break;
                    }
                }
                if (start < 0) {
                    start = 0;
                }
                r = decode(s(start, end), nbytes);
                if (start + nbytes != end) {
                    return nbytes=1, RuneError;
                }
                return r;
            }
            
            rune decode_last(str s) {
                int nbytes;
                return decode_last(s, nbytes);
            }
            
            size count(str s) {
                size count = 0;
                for (rune r : runes(s)) {
                    (void) r;
                    count++;
                }
                return count;
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
            
            bool full_rune(str s) {
                bool isshort;
                int nbytes;
                internal::decode_rune(s, nbytes, isshort);
                return !isshort;
            }
        } // uf8
    } // unicode
} // axe
#import "unicode.h"
#import "tables.h"

namespace axe { 
    namespace unicode { 
        namespace {
            // linearMax is the maximum size table for linear search for non-Latin1 rune.
            const int linearMax = 18;
            
            // is16 reports whether r is in the sorted slice of 16-bit ranges.
            template <typename Range>
            bool is_x(array<Range> ranges, rune r) {
                if (len(ranges) <= linearMax || r <= MaxLatin1) {
                    for (Range& range : ranges) {
                        if (r < range.lo)
                            return false;
                        if (r <= range.hi)
                            return (r-range.lo) % range.stride == 0;
                    }
                    return false;
                }
                
                // binary search over ranges
                size lo = 0;
                size hi = len(ranges);
                while (lo < hi) {
                    size m = lo + (hi-lo)/2;
                    Range& range = ranges[m];
                    if (range.lo <= r && r <= range.hi)
                        return (r-range.lo) % range.stride == 0;
                    if (r < range.lo)
                        hi = m;
                    else
                        lo = m + 1;
                }
                return false;
            }
    }

    bool is_excluding_latin(RangeTable const& rangetab, rune r) {
        array<Range16> r16 = rangetab.r16;
        int off = rangetab.latin_offset;
        if (len(r16) > off && r < r16[len(r16)-1].hi) {
            return is_x(r16(off), r);
        }
        array<Range32> r32 = rangetab.r32;
        if (len(r32) > 0 && r > r32[0].lo) {
            return is_x(r32, r);
        }
        return false;
    }

    }
}
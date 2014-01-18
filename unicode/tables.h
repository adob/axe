#import <axe/array.h>
namespace axe { 
    namespace unicode {
        
        struct Range16 {
            uint16 lo;
            uint16 hi;
            uint16 stride;
        } ;
        
        struct Range32 {
            uint32 lo;
            uint32 hi;
            uint32 stride;
        } ;
        
        
        struct RangeTable {
            const array<Range16> r16; 
            const array<Range32> r32;
            const int            latin_offset;
        } ;
        
        extern const RangeTable White_Space;
        
    }
}
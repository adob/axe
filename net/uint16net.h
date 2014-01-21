#import <netdb.h>
#import <axe/typedefs.h>
#import <axe/alloc.h>

namespace axe {
    namespace net {
        struct uint16net {
            uint16 val;
            uint16net() = default;
            
            uint16net(uint16 val) : val(htons(val)) {}
            operator uint16 () { return ntohs(val); }
            str string(Allocator& alloc) const;
        } ;
        
        inline bool operator == (uint16net a, uint16 b) {
            return ntohs(a) == b;
        }
        inline bool operator == (uint16 a, uint16net b) {
            return a == ntohs(b);
        }
        inline bool operator != (uint16net a, uint16 b) {
            return ntohs(a) != b;
        }
        inline bool operator != (uint16 a, uint16net b) {
            return a != ntohs(b);
        }
    }
}
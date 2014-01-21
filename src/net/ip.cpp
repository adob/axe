#import <axe/fmt.h>
#import "ip.h"
namespace axe {
    namespace net {
        
        str IPv4::string(Allocator& alloc) const {
            return fmt::sprintf(alloc, "%d.%d.%d.%d", addr8[0], addr8[1], addr8[2], addr8[3]);
        }
        
        str IPv6::string(Allocator& alloc) const {
            int e0 = -1, e1 = -1;
            
            for (int i = 0; i < len(addr); i += 2) {
                int j = i;
                while (j < len(addr) && addr[j] == 0 && addr[j+1] == 0)
                    j += 2;
                if (j > i && j-i > e1-e0)
                    e0 = i, e1 = j;
            }
            
            if (e1-e0 <= 2)
                e0 = -1, e1 = -1;
            
            Str s(alloc);
            for (int i = 0; i < len(addr); i += 2) {
                if (i == e0) {
                    s += "::";
                    i = e1;
                    if (i >= len(addr))
                        break;
                } else if (i > 0) {
                    s += ":";
                }
                s += fmt::sprintf(alloc, "%x%.2x", addr[i], addr[i+1]);
            }
            return s;
        }
        
        str IP::string(Allocator& alloc) const {
            switch (type) {
                case IPNil: return "nil";
                case IPv4:  return ipv4.string(alloc);
                case IPv6:  return ipv6.string(alloc);
            }
            assert(false, exceptions::AssertionFailed, "should not be here");
        }
    }
}
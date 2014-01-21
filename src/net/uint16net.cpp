#import "uint16net.h"
#import <axe/fmt.h>
namespace axe { 
    namespace net {
        str uint16net::string(Allocator& alloc) const {
            return fmt::sprintf(alloc, "%d", ntohs(val)); 
        }
        
    }
}
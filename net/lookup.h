#import <axe/adt/list.h>
#import "ip.h"
#import "addr.h"

namespace axe {
    namespace net {
        list<IP>   lookup_ip(str host, Allocator&, errorparam = {});
        list<Addr> lookup(str host, str port, Allocator&, errorparam = {});
        
        Addr resolve_inet_addr(str addr, errorparam = {});
        Addr resolve_inet_addr(str addr, int port, errorparam = {});
    }
}
#import <axe/typedefs.h>

namespace axe {
    namespace net {
        enum protocol : uint16 {
            ip,
            ip4,
            ip6,
            tcp,
            tcp4,
            tcp6,
            unix,
            stream,
            dgram
        } ;
    }
}
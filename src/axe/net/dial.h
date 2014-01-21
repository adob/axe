#import "conn.h"
#import "net.h"
namespace axe {
    namespace net {
        Conn dial(protocol, str addr, errorparam = {});
        Conn dial_tcp(Addr const& addr, errorparam = {});
    }
}
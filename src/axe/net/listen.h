#import <axe/errorparam.h>
#import <axe/str.h>
#import <axe/io/file.h>
#import "conn.h"
#import "addr.h"
#import "net.h"
namespace axe {
    namespace net {
        struct Listener : io::File {
            
            Listener() = default;
            explicit Listener(int sockfd) : io::File(sockfd) {}
            
            Conn accept(errorparam = {});
            Addr addr(errorparam = {});
        } ;
        
        Listener listen(protocol, str addr, errorparam = {});
        Listener listen_tcp(Addr const& addr, errorparam = {});
    }
}
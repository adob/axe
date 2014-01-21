#import <axe/typedefs.h>
#import <axe/io/file.h>
#import "addr.h"
namespace axe {
    namespace net {
        
        struct Conn : io::File {
            Addr remote_addr;
            
            size recv(buf buffer, int flags = 0, errorparam = {});
            
            size send(str string, int flags = MSG_NOSIGNAL, errorparam = {});
            
            void setsockopt(int level, int optname, int optval, errorparam = {});
            
            size read(buf buffer, errorparam err = {}) { 
                return recv(buffer, 0 , err);
            }
            
            size write(str string, errorparam err = {}) { 
                return send(string, MSG_NOSIGNAL, err); 
            }
        } ;
    }
}
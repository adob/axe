#import "listen.h"
#import "lookup.h"

namespace axe { 
    namespace net { 
        Conn Listener::accept(errorparam err) {
            Conn conn;
            socklen_t storage_size = sizeof(conn.remote_addr);
            
            conn.fd = ::accept(fd, (sockaddr *) &conn.remote_addr,  &storage_size);
            if (conn.fd == -1) {
                return err=errno, std::move(conn);
            }
            return std::move(conn);
        }
        
        Addr Listener::addr(errorparam err) {
            Addr addr;
            socklen_t addrlen = sizeof(addr);
            int ret = ::getsockname(fd, (sockaddr *) &addr, &addrlen);
            if (ret == -1) {
                err=errno;
            }
            return addr;
        }
        
        Listener listen_tcp(Addr const& addr, errorparam err) {
            int ret;
            
            int sockfd = ::socket(addr.family, SOCK_STREAM, 0);
            if (sockfd == -1) {
                return err=errno, Listener{-1};
            }
            
            int optval = 1;
            ret = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
            if (ret == -1) {
                return err=errno, Listener{-1};
            }
            
            ret = ::bind(sockfd, (sockaddr *) &addr, sizeof(addr));
            if (ret == -1) {
                return err=errno, Listener{-1};
            }
            
            
            ret = ::listen(sockfd, SOMAXCONN);
            if (ret == -1) {
                return err=errno, Listener{-1};
            }
            
            return Listener{sockfd};
            
        }
        
        Listener listen(protocol proto, str addrstr, errorparam err) {   
            switch (proto) {
                case tcp:
                case stream: {
                    Addr addr = resolve_inet_addr(addrstr, err);
                    if (err)
                        return Listener{};
                    
                    return listen_tcp(addr, err);
                    
                } break;
                default:
                    return err = "Unimplemented", Listener{};
            }
            
            //return Listener{};
        }
    }
}
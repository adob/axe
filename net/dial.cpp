#include <netdb.h>

#import "PKG.h"
namespace axe { namespace net { 

Conn dial(protocol proto, str addrstr, errorparam err) {
    switch (proto) {
    case tcp:
    case stream: {
        Addr addr = resolve_inet_addr(addrstr, err);
        if (err)
            return Conn{};
        
        return dial_tcp(addr, err);
    } break;
    default:
        return err = "Unimplemented", Conn{};
    }
}

Conn dial_tcp(Addr const& addr, errorparam err) {
    int ret;
    Conn conn;
    
    conn.fd = ::socket(addr.family, SOCK_STREAM, 0);
    if (conn.fd == -1) {
        return err=errno, std::move(conn);
    }
    
    ret = ::connect(conn.fd, (sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        return err=errno, std::move(conn);
    }
    
    conn.remote_addr = addr;
    return std::move(conn);
}

Listener tcp_listen(Addr const& addr, errorparam err) {
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
        
        return tcp_listen(addr, err);
        
    } break;
    default:
        return err = "Unimplemented", Listener{};
    }
    
    //return Listener{};
}
    
    
}}
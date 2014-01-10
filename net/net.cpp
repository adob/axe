#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#import <axe/net.h>
#import <axe/fmt.h>
#import <axe/print.h>

namespace axe {
namespace net {

//     // NOTE(rsc): In theory there are approximately balanced
//     // arguments for and against including AI_ADDRCONFIG
//     // in the flags (it includes IPv4 results only on IPv4 systems,
//     // and similarly for IPv6), but in practice setting it causes
//     // getaddrinfo to return the wrong canonical name on Linux.
//     // So definitely leave it out.
//     return C.AI_CANONNAME | C.AI_V4MAPPED | C.AI_ALL
    
str uint16net::string(Allocator& alloc) const {
    return fmt::sprintf(alloc, "%d", ntohs(val)); 
}
    
list<IP> lookup_ip(str host, Allocator& alloc, errorparam err) {
    List<IP> ips(alloc);
    
    addrinfo hints {
        ai_flags:     AI_ADDRCONFIG | AI_V4MAPPED | AI_ALL | AI_IDN,
        ai_family:    AF_UNSPEC,
        ai_socktype:  SOCK_STREAM, // if this is 0, we get a lot of duplicate entries
        ai_protocol:  0,
        ai_addrlen:   0,
        ai_addr:      nullptr,
        ai_canonname: nullptr,
        ai_next:      nullptr
    };
    
    addrinfo *results = nullptr;
    
    int gerrno = getaddrinfo(host.tmp_c_str(), nullptr, &hints, &results);
    
    auto free_later = defer([&]{ 
        if (results) {
            freeaddrinfo(results);
        }
    });
    
    if (gerrno) {
        if (gerrno == EAI_SYSTEM) {
            err = error(error::ErrNo, errno);
        } else {
            err = error(error::GAIErr, gerrno);
        }
    }
    for (addrinfo *result = results; result != nil; result = result->ai_next) {
        //print "got result len =", result->ai_addrlen, result->ai_addr->sa_family;
        
        switch (result->ai_addr->sa_family) {
        case AF_INET: {
            //print "GOT IPv4";
            sockaddr_in *ipv4addr = (sockaddr_in *) result->ai_addr;
            
            IP *ip          = ips.make();
            ip->type        = IP::IPv4;
            ip->ipv4.addr32 = ipv4addr->sin_addr.s_addr;
        } break;
        case AF_INET6: {
            //print "GOT IPv6";
            sockaddr_in6 *ipv6addr = (sockaddr_in6 *) result->ai_addr;
            
            IP *ip        = ips.make();
            ip->type      = IP::IPv6;
            memcpy(ip->ipv6.addr, ipv6addr->sin6_addr.s6_addr, sizeof(ip->ipv6.addr));
            
        } break;
        default:
            warn "Unknwon address type:", result->ai_addr->sa_family;
            break;
        }
    } 
    
    return ips;
}

list<Addr> lookup(str host, str service, Allocator& alloc, errorparam err) {
    List<Addr> addrs(alloc);
    
    addrinfo hints {
        ai_flags:     AI_ADDRCONFIG | AI_V4MAPPED | AI_ALL | AI_IDN,
        ai_family:    AF_UNSPEC,
        ai_socktype:  SOCK_STREAM, // if this is 0, we get a lot of duplicate entries
        ai_protocol:  0,
        ai_addrlen:   0,
        ai_addr:      nullptr,
        ai_canonname: nullptr,
        ai_next:      nullptr
    };
    
    addrinfo *results = nullptr;
    
    int gerrno = ::getaddrinfo(host.tmp_c_str(), service.tmp_c_str(), &hints, &results);
    
    auto free_later = defer([&]{ 
        if (results) {
            ::freeaddrinfo(results);
        }
    });
    
    if (gerrno) {
        if (gerrno == EAI_SYSTEM) {
            err = error(error::ErrNo, errno);
        } else {
            err = error(error::GAIErr, gerrno);
        }
    }
    for (addrinfo *result = results; result != nil; result = result->ai_next) {
        
        switch (result->ai_addr->sa_family) {
        case AF_INET: {                
                Addr *addr = addrs.make();
                //print "got one", ntohs(((sockaddr_in*)result->ai_addr)->sin_port);
                memcpy(addr, result->ai_addr, sizeof(*addr));
                //print addr->addr_ipv4.port.val;
                //print offsetof(Addr, addr_ipv4.port);
                //print offsetof(sockaddr_in, sin_port);
            } break;
        case AF_INET6: {
                Addr *addr = addrs.make();
                memcpy(addr, result->ai_addr, sizeof(*addr));
            } break;
        default:
            warn "Unknwon address type:", result->ai_addr->sa_family;
            break;
        }
    } 
    
    return addrs;
}

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
    assert(false);
}

str Addr::string(Allocator& alloc) const {
    switch (family) {
    case AF_INET:  return addr_ipv4.string(alloc);
    case AF_INET6: return addr_ipv6.string(alloc);
    }
    return "???";
}

size_t Addr::size() const {
    switch (family) {
    case AF_INET: return sizeof(struct sockaddr);
    case AF_INET6: return sizeof(AddrIPv6);
    }
    assert(false);
    return 0;
}

str AddrIPv4::string(Allocator& alloc) const {
    Str s(alloc);
    s += fmt::sprintf(alloc, "%s", addr);
    if (port != 0) 
        s += fmt::sprintf(alloc, ":%d", port);
    return s;
}

str AddrIPv6::string(Allocator& alloc) const {
    Str s(alloc);
    s += fmt::sprintf(alloc, "[%s]", addr, port);
    if (port != 0)
        s += fmt::sprintf(alloc, ":%d", port);
    if (flowinfo)
        s += fmt::sprintf(alloc, " (flow = %d)", flowinfo);
    if (scope_id)
        s += fmt::sprintf(alloc, " (scope = %d)", scope_id);
    
    return s;
}

Listener listen(Addr const& addr, errorparam err) {
    int ret;
    
    int sockfd = ::socket(addr.family, SOCK_STREAM, 0);
    if (sockfd == -1) {
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
    
    return Listener{-1};
    
}

buf Conn::recv(buf buffer, int flags, errorparam err) {
    size ret =::recv(sockfd, buffer.data, buffer.len, flags);
    if (ret == -1) {
        return err=errno, buf();
    }
    return buffer(0, ret);
}

Conn::~Conn() {
    if (sockfd != -1) {
        print "close1";
        int ret = close(sockfd);
        if (ret) {
            raise(errno);
        }
    }
    
}

Conn Listener::accept(errorparam err) {
    Conn conn;
    socklen_t storage_size = sizeof(conn.remote_addr);
    
    conn.sockfd = ::accept(3, (sockaddr *) &conn.remote_addr,  &storage_size);
    if (conn.sockfd == -1) {
        return err=errno, std::move(conn);
    }
    return std::move(conn);
}

Listener::~Listener() {
    if (sockfd != -1) {
        print "close2";
        int ret = close(sockfd);
        if (ret) {
            raise(errno);
        }
    }
    
}

namespace tcp {
    
Listener listen(Addr const&) {
    Listener listener;
    
    return listener;
}
    
}

}
}
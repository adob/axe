#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include "net.h"
#include "fmt.h"

namespace axe {
namespace net {

//     // NOTE(rsc): In theory there are approximately balanced
//     // arguments for and against including AI_ADDRCONFIG
//     // in the flags (it includes IPv4 results only on IPv4 systems,
//     // and similarly for IPv6), but in practice setting it causes
//     // getaddrinfo to return the wrong canonical name on Linux.
//     // So definitely leave it out.
//     return C.AI_CANONNAME | C.AI_V4MAPPED | C.AI_ALL
    
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
            
            IP *ip        = ips.make();
            ip->type      = IP::IPv4;
            uint32 addr = ipv4addr->sin_addr.s_addr;
            ip->ipv4.data = {(byte)  (addr      & 0xff), 
                             (byte) ((addr>>8)  & 0xff), 
                             (byte) ((addr>>16) & 0xff), 
                             (byte) ((addr>>24) & 0xff) };
        } break;
        case AF_INET6: {
            //print "GOT IPv6";
            sockaddr_in6 *ipv6addr = (sockaddr_in6 *) result->ai_addr;
            
            IP *ip        = ips.make();
            ip->type      = IP::IPv6;
            ip->ipv6.data = ipv6addr->sin6_addr.s6_addr;
            
        } break;
        default:
            warn "Unknwon address type:", result->ai_addr->sa_family;
            break;
        }
    } 
    
    return ips;
}

str IPv4::string(Allocator& alloc) const {
    return fmt::sprintf(alloc, "%d.%d.%d.%d", data[0], data[1], data[2], data[3]);
}

str IPv6::string(Allocator& alloc) const {
    int e0 = -1, e1 = -1;
    
    for (int i = 0; i < len(data); i += 2) {
        int j = i;
        while (j < len(data) && data[j] == 0 && data[j+1] == 0)
            j += 2;
        if (j > i && j-i > e1-e0)
            e0 = i, e1 = j;
    }
    
    if (e1-e0 <= 2)
        e0 = -1, e1 = -1;
    
    Str s(alloc);
    for (int i = 0; i < len(data); i += 2) {
        if (i == e0) {
            s += "::";
            i = e1;
            if (i >= len(data))
                break;
        } else if (i > 0) {
            s += ":";
        }
        s += fmt::sprintf(alloc, "%x%.2x", data[i], data[i+1]);
    }
    return s;
}

str IP::string(Allocator& alloc) const {
    ipv6.string(alloc);
    switch (type) {
        case IPNil: return "nil";
        case IPv4:  return ipv4.string(alloc);
        case IPv6:  return ipv6.string(alloc);
    }
    assert(false);
}
}
}
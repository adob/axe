#import <axe/net.h>
#import <axe/print.h>
#import <axe/fmt.h>
#import <axe/strings.h>

namespace axe { namespace net {


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
                assert(result->ai_addrlen <= sizeof(*addr));
                memcpy(addr, result->ai_addr, result->ai_addrlen);
                //print addr->addr_ipv4.port.val;
                //print offsetof(Addr, addr_ipv4.port);
                //print offsetof(sockaddr_in, sin_port);
            } break;
            case AF_INET6: {
                Addr *addr = addrs.make();
                assert(result->ai_addrlen <= sizeof(*addr));
                memcpy(addr, result->ai_addr, result->ai_addrlen);
            } break;
            default:
                warn "Unknwon address type:", result->ai_addr->sa_family;
                break;
        }
    } 
    
    return addrs;
}

void split_host_port(str hostport, str& host, str& port, errorparam err) {
    size j = 0, k = 0;
    
    if (len(hostport) == 0) {
        return err="Empty string";
    }
    
    size i = strings::rindex(hostport , ':');
    if (i == -1 || i == len(hostport)-1) {
        return err="Missing port in address";
    }
    
    if (hostport[0] == '[') {
        size end = strings::index(hostport, ']');
        if (end == -1) {
            return err="Missing ']' in address";
        }
        
        if (end+1 == len(hostport)) {
            return err="Missing port in address";
        } else if (end+1 != i) {
            if (hostport[end+1] == ':') {
                return err="Too many colons in address";
            }
            return err="Missing port in address";
        }
        host = hostport(1, end);
        j = 1, k = end+1;
    } else {
        host = hostport(0,i);
        if (strings::index(host, ':') != -1) {
            return err="Too many colons in address";
        }
        if (strings::index(host, '%') != -1) {
            return err="Missing brackets in address";
        }
    }
    if (strings::index(hostport(j), '[') != -1) {
        return err="unexpected '[' in address";
    }
    if (strings::index(hostport(k), ']') != -1) {
        return err="unexpected ']' in address";
    }
    
    port = hostport(i+1);
}

Addr resolve_inet_addr(str addr, errorparam err) {
    Allocator alloc;
    str host, port;
    
    split_host_port(addr, host, port, err);
    if (err) {
        return Addr();
    }
    
    list<Addr> addrs = lookup(host, port, alloc, err);
    if (err) {
        return Addr();
    }
    return first(addrs);
}

Addr resolve_inet_addr(str host, int port, errorparam err) {
    Allocator alloc;
    
    list<Addr> addrs = lookup(host, nil, alloc, err);
    if (err) {
        return Addr();
    }
    Addr addr = first(addrs);
    addr.addr_ip.port = port;
    return addr;
}

}}
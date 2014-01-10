#pragma once
#include <netdb.h>
#include <axe/core.h>

namespace axe {
namespace net {
    
    struct uint16net {
        uint16 val;
        uint16net() = default;
        
        uint16net(uint16 val) : val(htons(val)) {}
        operator uint16 () { return ntohs(val); }
        str string(Allocator& alloc) const;
    } ;
    
    inline bool operator == (uint16net a, uint16 b) {
        return ntohs(a) == b;
    }
    inline bool operator == (uint16 a, uint16net b) {
        return a == ntohs(b);
    }
    inline bool operator != (uint16net a, uint16 b) {
        return ntohs(a) != b;
    }
    inline bool operator != (uint16 a, uint16net b) {
        return a != ntohs(b);
    }
    
    struct IPv4 {
        union {
            uint32 addr32;
            uint8  addr8[4];
        } ;
        
        str string(Allocator&) const;
    } ;
    
    struct IPv6 {
        uint8  addr[16];  
        
        str string(Allocator&) const;
    } ;
    
    struct IP {
        enum IPType {
            IPNil = 0,
            IPv4  = 1,
            IPv6  = 2
        } type;
        
        union {
            net::IPv4 ipv4;
            net::IPv6 ipv6;
        } ;
        
        str string(Allocator&) const;
    };
    
    struct AddrIPv4 {
        sa_family_t family;
        uint16net   port;
        IPv4        addr;
        
        str string(Allocator&) const;
    } ;
    
    
    struct AddrIPv6 {
        sa_family_t family;
        uint16net   port;
        uint32      flowinfo;
        IPv6        addr;
        uint32      scope_id;
        
        str string(Allocator&) const;
    } ;
    
    struct Addr {
        
        union {
            sa_family_t family;
            AddrIPv4 addr_ipv4;
            AddrIPv6 addr_ipv6;
        } ;
        
        str string(Allocator&) const;
        size_t size() const;
    } ;
    
    list<IP>   lookup_ip(str host, Allocator&, errorparam = {});
    list<Addr> lookup(str host, str service, Allocator&, errorparam = {});
    
    struct Conn {
        int sockfd = -1;
        Addr remote_addr;
        
        Conn() = default;
        Conn(Conn const&) = delete;
        Conn(Conn&& other) : sockfd(other.sockfd) {
            other.sockfd = -1;
        }
        
        buf recv(buf buffer, int flags = 0, errorparam = {});
        
        Conn& operator = (Conn const&) = delete;
        
        ~Conn();
    } ;
    
    struct Listener {
        int sockfd = -1;
        
        Listener(int sockfd) : sockfd(sockfd) {}
        Listener(Listener const&) = delete;
        Listener(Listener&& other) : sockfd(other.sockfd) { other.sockfd = -1; }
        
        Conn accept(errorparam = {});
        
        Listener& operator = (Listener const&) = delete;
        
        ~Listener();
    } ;
    
    Listener listen(Addr const& addr, errorparam = {});
    
    namespace tcp {
        struct Listener {
            
        } ;
        
        Listener listen(Addr const& addr);
    } ;
    
}
}
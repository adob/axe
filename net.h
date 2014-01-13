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
            struct {
                sa_family_t family;
                uint16net   port;
            } addr_ip;
            
            AddrIPv4 addr_ipv4;
            AddrIPv6 addr_ipv6;
        } ;
        
        str string(Allocator&) const;
        size_t size() const;
    } ;
    
    enum protocol : ushort {
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
    
    
    list<IP>   lookup_ip(str host, Allocator&, errorparam = {});
    list<Addr> lookup(str host, str port, Allocator&, errorparam = {});
    
    void       split_host_port(str hostport, str& host, str& port, Allocator&, errorparam = {});
    
    //Addr&      resolve(str addr, protocol, Allocator&);
    Addr      resolve_inet_addr(str addr, errorparam = {});
    Addr      resolve_inet_addr(str addr, int port, errorparam = {});
    
    struct Conn {
        int sockfd = -1;
        Addr remote_addr;
        
        Conn() = default;
        Conn(Conn const&) = delete;
        Conn(Conn&& other) : sockfd(other.sockfd), remote_addr(other.remote_addr) {
            other.sockfd = -1;
        }
        
        buf recv(buf buffer, int flags = 0, errorparam = {});
        buf read(buf buffer, errorparam err) { return recv(buffer, 0 , err); }
        
        str send(str string, int flags = 0, errorparam = {});
        str write(str string, errorparam err) { return send(string, 0, err); }
        
        void close(errorparam = {});
        
        void setsockopt(int level, int optname, int optval, errorparam = {});
        
        Conn& operator = (Conn const&) = delete;
        Conn& operator = (Conn&& other) { 
            sockfd = other.sockfd;
            remote_addr = other.remote_addr;
            other.sockfd = -1; 
            return *this; 
        }
        
        ~Conn();
    } ;
    
    Conn dial(protocol, str adddr, errorparam = {});
    Conn dial_tcp(Addr const& addr, errorparam = {});
    
    struct Listener {
        int sockfd = -1;
        
        Listener() = default;
        Listener(int sockfd) : sockfd(sockfd) {}
        Listener(Listener const&) = delete;
        Listener(Listener&& other) : sockfd(other.sockfd) { other.sockfd = -1; }
        
        Conn accept(errorparam = {});
        
        Addr addr(errorparam = {});
        
        Listener& operator = (Listener const&) = delete;
        Listener& operator = (Listener&& other) { sockfd = other.sockfd; other.sockfd = -1; return *this; }
        
        ~Listener();
    } ;
    
    Listener listen(protocol, str addr, errorparam = {});
    //Listener listen(str addr, int port, protocol = stream, errorparam = {});
    Listener tcp_listen(Addr const& addr, errorparam = {});
    
    void split_host_port(str hostport, str& host, str& port, errorparam err);
}
}
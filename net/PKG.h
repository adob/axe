#include <netdb.h>
#import <axe/core.h>
#import <axe/io/PKG.h>

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
    
//     struct WatchInt {
//         int value;
//         
//         WatchInt(int i) : value(i) {}
//         
//         operator int () { return value; }
//         WatchInt& operator = (int other) {
//             printf("Change %d %d\n", value, other);
//             if (other == -4) {
//                 throw "WTF";
//             }
//             value = other;
//             return *this;
//         }
//     } ;
    
    struct Conn : io::File {
        Addr remote_addr;
        
        size recv(buf buffer, int flags = 0, errorparam = {});
        size read(buf buffer, errorparam err = {}) { return recv(buffer, 0 , err); }
        
        size send(str string, int flags = MSG_NOSIGNAL, errorparam = {});
        size write(str string, errorparam err = {}) { return send(string, MSG_NOSIGNAL, err); }
        
        void setsockopt(int level, int optname, int optval, errorparam = {});
    } ;
    
    Conn dial(protocol, str adddr, errorparam = {});
    Conn dial_tcp(Addr const& addr, errorparam = {});
    
    struct Listener : io::File {
        
        Listener() = default;
        explicit Listener(int sockfd) : io::File(sockfd) {}
        
        Conn accept(errorparam = {});
        Addr addr(errorparam = {});
    } ;
    
    Listener listen(protocol, str addr, errorparam = {});
    //Listener listen(str addr, int port, protocol = stream, errorparam = {});
    Listener tcp_listen(Addr const& addr, errorparam = {});
    
    void split_host_port(str hostport, str& host, str& port, errorparam err);
}
}
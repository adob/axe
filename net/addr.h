#include <netdb.h>
#import <axe/typedefs.h>
#import <axe/errorparam.h>
#import "uint16net.h"
#import "ip.h"

namespace axe {
    namespace net {
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
        
        void split_host_port(str hostport, str& host, str& port, errorparam err);
    }
}
        
        
        
        
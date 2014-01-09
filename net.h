#pragma once
#include <axe/core.h>

namespace axe {
namespace net {
    
    struct IPv4 {
        Array<byte, 4> data;
        
        str string(Allocator&) const;
    } ;
    
    struct IPv6 {
        Array<byte, 16> data;
        
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
    
    
    
    list<IP> lookup_ip(str host, Allocator&, errorparam = {});
    
}
}
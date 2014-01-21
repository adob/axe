#import <axe/typedefs.h>
#import <axe/alloc.h>
namespace axe {
    namespace net {
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
    }
}
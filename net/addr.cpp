#import <axe/fmt.h>
#import <axe/strings.h>
#import "addr.h"

namespace axe {
    namespace net {
        str Addr::string(Allocator& alloc) const {
            switch (family) {
                //case AF_UNSPEC:
                case AF_INET:  return addr_ipv4.string(alloc);
                case AF_INET6: return addr_ipv6.string(alloc);
            }
            return fmt::sprintf(alloc, "??? (family = %d)", family);
        }
        
        size_t Addr::size() const {
            switch (family) {
                case AF_INET: return sizeof(struct sockaddr);
                case AF_INET6: return sizeof(AddrIPv6);
            }
            assert(false, exceptions::AssertionFailed, "should not be here");
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
    }
}
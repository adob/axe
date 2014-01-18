#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>


#import <axe/fmt/fmt.h>
#import <axe/print.h>

#import "PKG.h"

namespace axe {
    namespace net {

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
            assert(false, exceptions::AssertionFailed, "should not be here");
        }

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

    }
}
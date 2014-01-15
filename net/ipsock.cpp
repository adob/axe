#include <netdb.h>
#include <unistd.h>

#import <axe/print.h>
#import "PKG.h"

namespace axe { namespace net { 

Conn Listener::accept(errorparam err) {
    Conn conn;
    socklen_t storage_size = sizeof(conn.remote_addr);
    
    conn.fd = ::accept(fd, (sockaddr *) &conn.remote_addr,  &storage_size);
    if (conn.fd == -1) {
        return err=errno, std::move(conn);
    }
    return std::move(conn);
}

Addr Listener::addr(errorparam err) {
    Addr addr;
    socklen_t addrlen = sizeof(addr);
    int ret = ::getsockname(fd, (sockaddr *) &addr, &addrlen);
    if (ret == -1) {
        err=errno;
    }
    return addr;
}

// Listener::~Listener() {
//     if (sockfd != -1) {
//         //print "close2";
//         int ret = close(sockfd);
//         if (ret) {
//             raise(errno);
//         }
//     }
//     
// }

//Conn::Conn(Conn&& other) : remote_addr(other.remote_addr) {}

size Conn::recv(buf buffer, int flags, errorparam err) {
    //print "BAD", sockfd.value;
    size ret = ::recv(fd, buffer.data, buffer.len, flags);
    if (ret == -1) {
        return err=errno, 0;
    }
    return ret;
}

size Conn::send(str string, int flags, errorparam err) {
    size ret = ::send(fd, string.data, string.len, flags);
    if (ret == -1) {
        return err=errno, 0;
    }
    return ret;
}

void Conn::setsockopt(int level, int optname, int optval, errorparam err) {
    int ret = ::setsockopt(fd, level, optname, &optval, sizeof (optval));
    if (ret) {
        return err=errno;
    }
}

// Conn& Conn::operator = (Conn&& other) { 
//     sockfd = other.sockfd;
//     remote_addr = other.remote_addr;
//     other.sockfd = -1; 
//     //printf("%d\n", sockfd);
//     return *this; 
// }



}}
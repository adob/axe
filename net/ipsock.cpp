#include <netdb.h>
#include <unistd.h>

#import <axe/print.h>
#import "PKG.h"

namespace axe { namespace net { 

Conn Listener::accept(errorparam err) {
    Conn conn;
    socklen_t storage_size = sizeof(conn.remote_addr);
    
    conn.sockfd = ::accept(sockfd, (sockaddr *) &conn.remote_addr,  &storage_size);
    if (conn.sockfd == -1) {
        return err=errno, std::move(conn);
    }
    return std::move(conn);
}

Addr Listener::addr(errorparam err) {
    Addr addr;
    socklen_t addrlen = sizeof(addr);
    int ret = ::getsockname(sockfd, (sockaddr *) &addr, &addrlen);
    if (ret == -1) {
        err=errno;
    }
    return addr;
}

Listener::~Listener() {
    if (sockfd != -1) {
        //print "close2";
        int ret = close(sockfd);
        if (ret) {
            raise(errno);
        }
    }
    
}

Conn::Conn(Conn&& other) : sockfd(other.sockfd), remote_addr(other.remote_addr) {
    static int count = -1;
    other.sockfd = count--;
    //printf("%d\n", sockfd);
}

size Conn::recv(buf buffer, int flags, errorparam err) {
    //print "BAD", sockfd.value;
    size ret = ::recv(sockfd, buffer.data, buffer.len, flags);
    if (ret == -1) {
        return err=errno, 0;
    }
    return ret;
}

size Conn::send(str string, int flags, errorparam err) {
    size ret = ::send(sockfd, string.data, string.len, flags);
    if (ret == -1) {
        return err=errno, 0;
    }
    return ret;
}

void Conn::setsockopt(int level, int optname, int optval, errorparam err) {
    int ret = ::setsockopt(sockfd, level, optname, &optval, sizeof (optval));
    if (ret) {
        return err=errno;
    }
}

void Conn::close(errorparam err) {
    if (sockfd == -1) {
        return err="Already closed";
    }
    
    int ret = ::close(sockfd);
    if (ret) {
        return err=errno;
    }
}

Conn& Conn::operator = (Conn&& other) { 
    sockfd = other.sockfd;
    remote_addr = other.remote_addr;
    other.sockfd = -1; 
    //printf("%d\n", sockfd);
    return *this; 
}

Conn::~Conn() {
    if (sockfd != -1) {
        int ret = ::close(sockfd);
        if (ret) {
            raise(errno);
        }
    }
}




}}
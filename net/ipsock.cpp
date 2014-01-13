#include <netdb.h>
#include <unistd.h>

#import <axe/print.h>
#import "../net.h"

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

buf Conn::recv(buf buffer, int flags, errorparam err) {
    size ret =::recv(sockfd, buffer.data, buffer.len, flags);
    if (ret == -1) {
        return err=errno, buf();
    }
    return buffer(0, ret);
}

str Conn::send(str string, int flags, errorparam err) {
    size ret = ::send(sockfd, string.data, string.len, flags);
    if (ret == -1) {
        return err=errno, buf();
    }
    return string(0, ret);
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

Conn::~Conn() {
    if (sockfd != -1) {
        //print "close1", this, sockfd;
        int ret = ::close(sockfd);
        if (ret) {
            raise(errno);
        }
    }
}




}}
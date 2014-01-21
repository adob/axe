#import "conn.h"
namespace axe { 
    namespace net { 
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
    }
}
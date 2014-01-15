#include <stdio.h>
#include <unistd.h>

#import <axe/core.h>

namespace axe {
namespace io {
    
    struct File {
        int fd = -1;
        
        File() = default;
        explicit File(int fd) : fd(fd) {}
        File(File const&) = delete;
        File(File&& other) : fd(other.fd) { other.fd = -1; }
        
        File& operator = (File const&) = delete;
        File& operator = (File&& other) {
            fd = other.fd;
            other.fd = -1;
            return *this;
        }
        
        void close(errorparam err);
        
        ~File() {
            if (fd != -1) {
                int ret = ::close(fd);
                if (ret) {
                    raise(errno);
                }
            }
        }
    };
    
    size write(FILE *f, str s, errorparam = {});
    size write(int fd, str s, errorparam = {});
    
    size read(FILE *, buf b, errorparam  = {});
    size read(int fd, buf b, errorparam = {});
    
    template <typename Writer>
    size write(Writer&& w, str s) {
        return w.write(s);
    }
    
    template <typename Writer>
    size write(Writer&& w, str s, errorparam err) {
        return w.write(s, err);
    }
    
    template <typename Reader>
    size read(Reader&& r, buf b, errorparam err = {}) {
        return r.read(b, err);
    }
    
    template <typename Reader>
    size read_full(Reader&& reader, buf buffer, errorparam err = {}) {
        size n = 0;
        size min = len(buffer);
        do {
            size nn = io::read(reader, buffer(n), err);
            if (err) {
                return n+nn;
            } else if (nn == 0) {
                err = EOF;
                return n;
            }
            n += nn;
        } while (n < min);
        return n;
    }
}
}

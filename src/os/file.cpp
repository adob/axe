#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
       
#include "file.h"
#include "print.h"

namespace axe {
namespace os {

File create(str name, int flag, mode_t perm) {
    return open(name, flag, perm);
    
}
File create(const char *name, int flag, mode_t perm) {
    return open(name, flag, perm);
    
}

void File::init(int fd, str name) {
    this->err  = OK;
    this->fd   = fd;
    this->name = name;
}


size File::read(bufref b, error& e) {
    if (this->err == ErrInvalid) {
        return e=ErrInvalid, 0;
    } 
    size count = ::read(fd, b.data, b.len);
    if (count == (size) -1) {
        return e=errno, 0;
    } else if (count == 0 && b.len > 0) {
        return e=eof, 0;
    }
    return e=nil, count;
}


size File::pread_fully(bufref b, size offset, error& e) {
    if (this->err == ErrInvalid) {
        err = ErrInvalid;
        return e=err, 0;
    } 
    size n = 0;
    while (b.len > 0) {
        size m = ::pread(fd, b.data, b.len, offset);
        if (m == 0) {
            err = eof;
            return e=err, n;
        } else if (m == (size) -1) {
            err = errno;
            return e=err, n;
        }
        n += m;
        b = b.slice(m);
        offset += m;
    }
    err = nil;
    return e=nil, n;
}


File open(str name, int flag, mode_t perm) {
    char buf[PATH_MAX];
    
    if (name.len >= PATH_MAX) {
        File file;
        file.err = ENAMETOOLONG;
        return file;
    }
    memcpy(buf, name.data, name.len);
    buf[name.len] = '\0';
    return open(buf, flag, perm);
}
File open(const char *name, int flag, mode_t perm) {
    File file;
    int r = ::open(name, flag, perm);
    if (r == -1) {
        file.fd  = -1;
        file.err = errno;
        return file;
    }
    file.init(r, name);
    file.err = OK;
    return file;
}

} // namespace os
}
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
       
#include "file.h"
#include "print.h"

namespace os {

File create(str name, error & err) {
    File file;
    err = create(name, file);
    return file;
}
File create(str name) {
    error err;
    return create(name, err);
}
error create(str name, File& file) {
    return open_file(name, O_RDWR|O_CREAT|O_TRUNC, 0666, file);
}

File open_file(str name, int flag, mode_t perm, error& err) {
    File file;
    err = open_file(name, flag, perm, file);
    return file;
}
File open_file(str name, int flag, mode_t perm) {
    error err;
    return open_file(name, flag, perm, err);
}
error open_file(str name, int flag, mode_t perm, File& file) {
    static_assert(sizeof(thread_local_buf) > PATH_MAX, "buffer too small");
    
    if (name.len >= PATH_MAX) {
        return ENAMETOOLONG;
    }
    memcpy(thread_local_buf, name.data, name.len);
    thread_local_buf[name.len] = '\0';
    
    int r = ::open(thread_local_buf, flag|O_CLOEXEC, perm);
    if (r == -1) {
        file.err = errno;
        return errno;
    }
    file.init(r, name);
    return OK;
}

void File::init(int fd, str name) {
    this->err  = OK;
    this->fd   = fd;
    this->name = name;
}
size File::read(byteref b) {
    error e;
    return read(b, e);
}
size File::read(byteref b, error& err) {
    size count = ::read(fd, b.data, b.len);
    if (count == -1u) {
        err = errno;
        return 0;
    } else if (count == 0 && b.len > 0) {
        return 0;
    }
    return 0;
}

File open(str name, error& err) {
    File file;
    err = open(name, file);
    return file;
}
File open(str name) {
    error err;
    return open(name, err);
}

error open(str name, File& file) {
    return open_file(name, O_RDONLY, 0, file);
}
    
}

#include <fcntl.h>
#include <unistd.h>
       
#include "file.h"
#include "print.h"

namespace alex {
namespace os {

File create(str name, error & err) {
    File file;
    err = create(name, file);
    return file;
}
File create(str name) {
    syscall::error err;
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
    syscall::error err;
    return open_file(name, flag, perm, err);
}
error open_file(str name, int flag, mode_t perm, File& file) {
    syscall::error err;
    int r = syscall::open(name, flag|O_CLOEXEC, perm, err);
    if (err) {
        return err;
    }
    file.init(r, name);
    return err;
}

void File::init(int fd, str name) {
    this->fd   = fd;
    this->name = name;
}
size File::read(slice<char> b) {
    error e;
    return read(b, e);
}
size File::read(slice<char> b, os::error& err) {
    size count = ::read(fd, b.data, b.len);
    if (count < 0) {
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
}
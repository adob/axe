#include <fcntl.h>
#include <string.h>
#include <ostream>

#include "syscall.h"

#include "print.h"

namespace alex {
namespace syscall {

int open(str path, int mode, uint32 perm, error& err) {
    int i = ::open(path.data, mode|O_LARGEFILE, perm);
    if (i == -1) {
        err = errno;
    } else {
        err = OK;
    }
    return i;
}

error::operator alex::error () {
    return "syscall::error";
}

std::ostream& operator<< (std::ostream& o, error err) {
    char buf[256];
    o << strerror_r(err, buf, sizeof buf);
    return o;
}

}
}
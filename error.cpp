#include "error.h"

std::ostream& operator<< (std::ostream& o, error err) {
    char buf[256];
    o << strerror_r(err, buf, sizeof buf);
    return o;
}
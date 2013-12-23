#include <string.h>
#include <ostream>

#include "builtin.h"
#include "error.h"
#include "print.h"

std::ostream& operator<< (std::ostream& stream, error err) {
    if (err.is_errno) {
        char buf[1024];
        stream << strerror_r(err.code, buf, sizeof buf);
        // result may be truncated but strerror_r() guarantees 
        // it is NUL-terminated
    } else if (err.is_funcp) {
        print "!! not implemented!!";
    } else {
        stream << err.charp;
    }

    return stream;
}
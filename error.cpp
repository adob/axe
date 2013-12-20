#include <string.h>
#include <ostream>

#include "builtin.h"
#include "error.h"
#include "print.h"

std::ostream& operator<< (std::ostream& stream, error err) {
    if (err.is_errno) {
            stream << err.charp;
    } else if (err.is_funcp) {
        print "!! not implemented!!";
    } else {
        stream << strerror_r(err.code, thread_local_buf, sizeof thread_local_buf); 
        // result may be truncated but strerror_r() guarantees 
        // it is NUL-terminated
    }

    return stream;
}
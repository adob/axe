#include <string.h>
#include <ostream>
#include <netdb.h>

#include <axe/core.h>
#include <axe/print.h>
#include <axe/debug.h>

namespace axe {

std::ostream& operator<< (std::ostream& stream, error err) {
    
    if (err.type == error::StringErr && err.charp) {
        stream << err.charp;
    } else if (err.type == error::ErrNo) {
        char buf[1024];
        stream << strerror_r(err.code, buf, sizeof buf);
        // result may be truncated but strerror_r() guarantees 
        // it is NUL-terminated
    } else  if (err.type == error::StringErr){
        stream << err.charp;
    } else if (err.type == error::GAIErr) {
        stream << gai_strerror(err.code);
    }

    return stream;
}

void raise(str msg) {
    Exception ex { debug::backtrace(2), (std::string) msg };
    throw ex;
}

void raise(int err) {
    Exception ex { debug::backtrace(2), std::string(strerror(err)) };
}
}
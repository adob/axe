#include <string.h>
#include <ostream>
#include <netdb.h>

#import "PKG.h"
#include <axe/print.h>
#include <axe/debug/PKG.h>

namespace axe {

std::ostream& operator<< (std::ostream& stream, error err) {
    
    if (err.type == error::StringErr && err.charp) {
        stream << err.charp;
    } else if (err.type == error::ErrNo) {
        if (err.code == -1) {
            stream << "EOF";
            return stream;
        }
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

str error::string(Allocator&) const {
    if (type == error::StringErr && charp) {
        return str(charp);
    } else if (type == error::ErrNo) {
        if (code == -1) {
            return "EOF";
        }
        char buf[1024];
        return str(strerror_r(code, buf, sizeof buf));
        // result may be truncated but strerror_r() guarantees 
        // it is NUL-terminated
    } else  if (type == error::StringErr){
        return str(charp);
    } else if (type == error::GAIErr) {
        return str(gai_strerror(code));
    }
    return "???";
}

str errorparam::string(Allocator& alloc) const {
    if (err) {
        return err->string(alloc);
    } else {
        return "nil";
    }
}

void raise(str msg) {
    Exception ex { debug::backtrace(2), (std::string) msg };
    throw ex;
}

void raise(int err) {
    Exception ex { debug::backtrace(2), std::string(strerror(err)) };
}
}
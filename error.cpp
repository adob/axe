#include <string.h>
#include <ostream>
#include <netdb.h>

#include <axe/print.h>
#include <axe/debug/debug.h>
#include <axe/fmt/fmt.h>
#import "error.h"

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
    char buf[1024];
    
    if (type == error::StringErr && charp) {
        return str(charp);
    } else if (type == error::ErrNo) {
        if (code == -1) {
            return "EOF";
        }
        return str(strerror_r(code, buf, sizeof buf));
        // result may be truncated but strerror_r() guarantees 
        // it is NUL-terminated
    } else  if (type == error::StringErr){
        if (charp == nullptr) {
            return strerror_r(0, buf, sizeof buf);
        }
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

Exception::Exception() {
    backtrace = debug::backtrace(0);
}

void raise(str msg) {
    Exception ex;
    ex.msg = msg;
    throw ex;
}

void raise(const char* strp) {
    raise(strp);
}

void raise(int err) {
    Exception ex;
    ex.msg = strerror(err);
    throw ex;
}

void raise(error e) {
    Exception ex;
    ex.msg = fmt::sprintf("Error: %s", e);
    throw ex;
}

}
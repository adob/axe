#pragma once
#include <axe/core.h>

#include <stdio.h>

namespace axe {
namespace io {

    const error ERR_SHORT_WRITE = "short write";
    const error ERR_SHORT_BUFFER = "short buffer";

    const error eof = "EOF";
    
    struct IReader {
        virtual size read(bufref p, error& err) = 0;
    };
    
    template <typename Writer, typename T>
    auto write(Writer&& w, T&& s, errorparam err = {}) {
        return w.write(std::forward<T>(s), err);
    }
    
    
    str write(FILE *f, str s, errorparam = {});
    str write(int fd, str s, errorparam = {});
    
    buf read(FILE *, buf b, errorparam  = {});
    buf read(int fd, buf b, errorparam = {});
    
    template <typename T>
    buf read(T&& t, buf b, errorparam err = {}) {
        return t.read(b, err);
    }
    
    template <typename Reader>
    buf read_full(Reader&& reader, buf buffer, errorparam err = {}) {
        size n = 0;
        size min = len(buffer);
        do {
            size nn = io::read(reader, buffer(n), err).len;
            if (err) {
                return buffer(0, n+nn);
            } else if (nn == 0) {
                err = EOF;
                return buffer(0, n);
            }
            n += nn;
        } while (n < min);
        return buffer(0, n);
    }
}
}

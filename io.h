#pragma once
#include <axe/core.h>

#include <stdio.h>

namespace axe {
namespace io {

    const error ERR_SHORT_WRITE = "short write";
    const error ERR_SHORT_BUFFER = "short buffer";

    struct IReader {
        virtual size read(bufref p, error& err) = 0;
    };
    
    template <typename Writer>
    size write(Writer& w, str s, error& err) {
        return w.write(s, err);
    }
}

esize write(FILE *f, str s);
}

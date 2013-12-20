#pragma once
#include "builtin.h"

namespace io {

    const error ERR_SHORT_WRITE = "short write";
    const error ERR_SHORT_BUFFER = "short buffer";

    struct IReader {
        virtual size read(buf& p, error& err) = 0;
    };

}
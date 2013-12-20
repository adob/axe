#pragma once
#include "datatypes.h"
#include "buf.h"

namespace alex {
namespace io {

const error ERR_SHORT_WRITE = "short write";
const error ERR_SHORT_BUFFER = "short buffer";
const error EOF_ = "EOF";

struct IReader {
    virtual size read(buf& p, error& err) = 0;
};



}
}
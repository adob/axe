#pragma once
#include <axe/core.h>

namespace axe {
namespace time {
    // Returns a monotonicly-inreasing timestamp in nanoseconds since an
    // unspecified starting point
    int128 monotonic();
    
    // Returns nanoseconds elpased since the UNIX Epoch
    int128 now();
}
}
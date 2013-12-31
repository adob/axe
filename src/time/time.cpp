#include <axe/time.h>

namespace axe {
namespace time {
    int128 monotonic() {
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000000000 + ts.tv_nsec;
    }
    
    int128 now() {
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec * 1000000000 + ts.tv_nsec;
    }
}
}
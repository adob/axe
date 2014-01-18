#include <pthread.h>
#import <axe/raise.h>
#import "barrier.h"

namespace axe {
    namespace sync {
        Barrier::Barrier(unsigned count) {
            pthread_barrierattr_t attr;
            if (pthread_barrierattr_init(&attr))
                raise(errno);
            
            if (pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE)) {
                pthread_barrierattr_destroy(&attr);
                raise(errno);
            }
            
            if (pthread_barrier_init(&barr, &attr, count))
                raise(errno);
            
            pthread_barrierattr_destroy(&attr);
        }
        
        void Barrier::wait() {
            int ret = pthread_barrier_wait(&barr);
            if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD)
                raise(errno);
        }
        
        Barrier::~Barrier() {
            pthread_barrier_destroy(&barr);
        }
    }

}
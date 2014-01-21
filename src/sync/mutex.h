#include <pthread.h>

namespace axe {
    namespace sync {
        struct Mutex {
            pthread_mutex_t mutex;
            
            Mutex();
            Mutex(Mutex&&) = delete;
            void lock();
            void unlock();
            ~Mutex();
        } ;
    }
}
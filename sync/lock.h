#import <axe/raise.h>
#import "mutex.h"

namespace axe {
    namespace sync {
        struct Lock {
            Mutex& mutex;
            bool locked;
            
            Lock(Mutex&);
            void relock();
            void unlock();
            ~Lock();
        } ;
    }
}
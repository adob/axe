#include <pthread.h>
#import "mutex.h"

namespace axe {
    namespace sync {
        struct CondVar {
            pthread_cond_t cond;
            
            CondVar();
            void wait(Mutex&);
            void signal();
            void broadcast();
            ~CondVar();
        } ;
    }
}

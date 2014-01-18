#import <axe/raise.h>
#import "mutex.h"
#import "condvar.h"

namespace axe {
    namespace sync {
        CondVar::CondVar() {
            pthread_condattr_t attr;
            
            if (int code = pthread_condattr_init(&attr))
                raise(code);
            
            if (int code = pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE)) {
                pthread_condattr_destroy(&attr);
                raise(code);
            }
            
            if (int code = pthread_cond_init(&cond, &attr)) {
                pthread_condattr_destroy(&attr);
                raise(code);
            }
            
            if (int code = pthread_condattr_destroy(&attr)) {
                raise(code);
            }
        }
        
        void CondVar::wait(Mutex& mutex) {
            if (int code = pthread_cond_wait(&cond, &mutex.mutex))
                raise(code);
        }
        
        void CondVar::signal() {
            if (int code = pthread_cond_signal(&cond))
                raise(code);
        }
        
        void CondVar::broadcast() {
            if (int code = pthread_cond_broadcast(&cond))
                raise(code);
        }
        
        CondVar::~CondVar() {
            if (int code = pthread_cond_destroy(&cond)) {
                raise(code);
            }
        }
    }
}
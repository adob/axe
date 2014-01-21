#import "mutex.h"
#import <axe/raise.h>
//#import <axe/print.h>
//#import <axe/debug/debug.h>

namespace axe {
    namespace sync {
        Mutex::Mutex() {
            pthread_mutexattr_t attr;
            
            if (int code = pthread_mutexattr_init(&attr)) {
                raise(code);
            }

            if (int code = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE)) {
                pthread_mutexattr_destroy(&attr);
                raise(code);
            }

            //         if (pthread_mutexattr_settype&attr, PTHREAD_MUTEX_DEFAULT) {
            //             pthread_mutexattr_destory(&attr);
            //             throw error(errno);
            //         }
            //         
            //         if (pthread_mutexattr_setprotocol&attr, PTHREAD_PRIO_NONE)) {
            //             pthread_mutexattr_destory(&attr);
            //             throw error(errno);
            //         }

            if (int code = pthread_mutex_init(&mutex, &attr)) {
                pthread_mutexattr_destroy(&attr);
                raise(code);
            }

            if (int code = pthread_mutexattr_destroy(&attr)) {
                raise(code);
            }
            //print "create";
        }

        void Mutex::lock() {
            //print "lock";
            //debug::print_backtrace();
            if (int code = pthread_mutex_lock(&mutex)) {
                raise(code);
            }
                
        }

        void Mutex::unlock() {
            //print "unlock";
            if (int code = pthread_mutex_unlock(&mutex)) {
                raise(code);
            }
        }
            
        Mutex::~Mutex() {
            //print "destroy";
            if (int code = pthread_mutex_destroy(&mutex)) {
                raise(code);
            }
        }
    }
}
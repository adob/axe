#import "lock.h"

namespace axe {
    namespace sync {
        
        Lock::Lock(Mutex& mutex) : mutex(mutex), locked(true) {
            mutex.lock();
        }
        
        void Lock::relock() {
            if (locked)
                raise("Lock already locked");
            locked = true;
            mutex.lock();
        }
        
        void Lock::unlock() {
            if (!locked)
                raise("Lock already unlocked");
            
            mutex.unlock();
            locked = false;
        }
        
        Lock::~Lock() {
            if (locked)
                mutex.unlock();
        }
    }
}

#import <queue>
#import <axe/typedefs.h>
#import "condvar.h"
#import "lock.h"
namespace axe {
    namespace sync {
        const int MaxQueueSize = 100000;
        
        template <typename T>
        struct Queue {
            std::queue<T>  queue;
            CondVar        enqcond;
            CondVar        deqcond;
            Mutex          mutex;
            const size     max_size;
            
            Queue(size max_size=MaxQueueSize);
            
            void enqueue(T const& t);
            void enqueue(T && t);
            
            T dequeue();
            
            Queue(Queue&&) = delete;
        } ;
        
        #import "queue.inlines.h"
    }
}


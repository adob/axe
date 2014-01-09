#include <axe/core.h>
#include <functional>
#include <queue>

namespace axe {
namespace sync {
    const int MaxQueueSize = 100000;
    
    void init(errorparam err = {});
    
    void go(std::function<void()> const& f);
    void go(std::function<void()>     && f);
    
    struct Mutex {
        pthread_mutex_t mutex;
        
        Mutex();
        void lock();
        void unlock();
        ~Mutex();
    } ;
    
    struct Lock {
        Mutex& mutex;
        bool locked;
        
        Lock(Mutex&);
        void relock();
        void unlock();
        ~Lock();
    } ;
    
    struct Barrier {
        pthread_barrier_t barr;
        
        Barrier(unsigned count);
        void wait();
        ~Barrier();
    } ;
    
    struct Cond {
        pthread_cond_t cond;
        
        Cond();
        void wait(Mutex&);
        void signal();
        ~Cond();
    } ;
    
    template <typename T>
    struct Queue {
        std::queue<T>  queue;
        Cond           enqcond;
        Cond           deqcond;
        Mutex          mutex;
        const size     max_size;
        
        Queue(size max_size=MaxQueueSize);
        
        void enqueue(T const& t);
        void enqueue(T && t);
        
        T dequeue();
    } ;
    
    struct Guard {
        Mutex mutex;
        Cond  cond;
        bool  value = false;
        
    } ;
    
    struct GuardList {
        Guard     *guard;
        GuardList *next;
        GuardList *prev;
    } ;
    
    template <typename T>
    struct Chan  {
        std::queue<T>  queue;
        Cond           enqcond;
        Cond           deqcond;
        Mutex          mutex;
        const size     max_size;
        GuardList      *guards = nullptr;
        bool           closed  = false;
        
        Chan(size max_size=MaxQueueSize);
        
        void send(T const& t);
        void send(T && t);
        
        T    recv();
        bool recv(T&);
        
        bool poll(T&);
        
        void close();
        
    } ;
    
    template <typename T, typename... Args>
    bool wait(Chan<T>& chan, Args&...);
    
    template <typename RetType, typename T, typename... Args>
    RetType select(RetType ret, sync::Chan<T>& chan, T& item, Args&&... args);
    
    
    #include "sync/sync.inlines.h"
}
}
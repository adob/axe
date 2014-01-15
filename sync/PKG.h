#include <queue>
#include <sys/epoll.h>
#import <axe/core.h>
#import <axe/io/PKG.h>

namespace axe {
namespace sync {
    const int MaxQueueSize = 100000;
    
    void init(errorparam err = {});
    
    void go(Func<void()> const& f);
    void go(Func<void()>     && f);
    
    struct Mutex {
        pthread_mutex_t mutex;
        
        Mutex();
        Mutex(Mutex&&) = delete;
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
        void broadcast();
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
        //Chan(Chan&&);
        
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
    
    const int IN = EPOLLIN;
    const int OUT = EPOLLOUT;
    
    void notify_init(errorparam err = {});
    
    struct Notifier {
        int fd;
        int op;
        Chan<uint32> *chan;
        
        //Notifier();
        Notifier(io::File& file, int op, Chan<uint32>& chan, errorparam = {});
        Notifier(int fd, int op, Chan<uint32>& chan, errorparam = {});
        Notifier(Notifier const&) = delete;
        Notifier(Notifier&&);
        
        //void add(int op, sync::Chan<uint32>& chan, errorparam = {});
        void modify(int op, Chan<uint32>& chan, errorparam = {});
        void modify(int op, errorparam = {});
        void rearm(errorparam = {});
        void stop(errorparam = {});
        
        Notifier& operator = (Notifier const&) = delete;
        Notifier& operator = (Notifier     &&);
        
        ~Notifier();
    } ;
    
    void notify(io::File&, uint32 op, sync::Chan<uint32>& chan, errorparam = {});
    void notify(int fd, uint32 op, sync::Chan<uint32>& chan, errorparam = {});
    
    void renotify(io::File&, uint32 op, sync::Chan<uint32>& chan, errorparam = {});
    void renotify(int fd, uint32 op, sync::Chan<uint32>& chan, errorparam = {});
    
    void unnotify(io::File&, errorparam = {});
    void unnotify(int fd, errorparam = {});
    
    //void notify_once(io::File&, int op, sync::Chan<uint32>& chan, errorparam = {});
    //void notify(io::File&, int op, errorparam = {});
    
}
}

#import "sync.inlines.h"
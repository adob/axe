#include <queue>

#import <axe/raise.h>
#import "mutex.h"
#import "condvar.h"
#import "lock.h"

namespace axe {
    namespace sync {
        namespace internal {
            struct Guard {
                Mutex   mutex;
                CondVar cond;
                bool    value = false;
                
            } ;
            
            struct GuardList {
                Guard     *guard;
                GuardList *next;
                GuardList *prev;
            } ;
        } // namespace internal
        using namespace internal;
        
        const int MaxChanSize = 100000;
        
        template <typename T>
        struct Chan  {
            
            
            std::queue<T>  queue;
            CondVar        enqcond;
            CondVar        deqcond;
            Mutex          mutex;
            const size     max_size;
            GuardList     *guards    = nullptr;
            bool           closed    = false;
            
            Chan(size max_size=MaxChanSize);
            
            void send(T const& t);
            void send(T && t);
            
            T    recv();
            bool recv(T&);
            
            bool poll(T&);
            
            void close();
            
            Chan(Chan&&) = delete;
        } ;
        
        template <typename T, typename... Args>
        bool wait(Chan<T>& chan, Args&...);
        
        template <typename RetType, typename T, typename... Args>
        RetType select(RetType ret, Chan<T>& chan, T& item, Args&&... args);
        
        #import "chan.inlines.h"
    }
}


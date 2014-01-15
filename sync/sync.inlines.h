#import <axe/print.h>

namespace axe { namespace sync {

template <typename T>
Queue<T>::Queue(size max_size)
  : max_size(max_size)
  {}

template <typename T>
void Queue<T>::enqueue(T const& t) {
    {
        Lock lock(mutex);
        while (queue.size() > max_size) {
            enqcond.wait(mutex);
        }
        queue.push(t);
    }
    deqcond.signal();
}

template <typename T>
void Queue<T>::enqueue(T && t) {
    {
        Lock lock(mutex);
        while (queue.size() > max_size) {
            enqcond.wait(mutex);
        }
        queue.push(std::move(t));
    }
    deqcond.signal();
}


template<typename T>
T Queue<T>::dequeue() {
    Lock lock(mutex);
    
    while (queue.empty()) {
        deqcond.wait(mutex);
    }
    
    T item = std::move(queue.front());
    size oldqueuesize = queue.size();
    queue.pop();
    
    lock.unlock();
    
    
    if (oldqueuesize == max_size) {
        enqcond.signal();
    }
    
    return std::move(item);
}

template <typename T>
Chan<T>::Chan(size max_size)
  : max_size(max_size)
  {}
  
template <typename T>
void Chan<T>::send(T const& t) { 
    {
        Lock lock(mutex);
        if (closed)
            raise("Can't send on a closed channel");
        
        while (queue.size() > max_size) {
            enqcond.wait(mutex);
        }
        queue.push(t);
        
        if (guards) {
            Guard *guard = guards->guard;
            Lock guardlock(guard->mutex);
            guard->value = true;
            guardlock.unlock();
            guard->cond.signal();
        }
    }
    deqcond.signal();
}

template <typename T>
void Chan<T>::send(T && t) {
    {
        Lock lock(mutex);
        if (closed)
            raise("Can't send on a closed channel");
        
        while (queue.size() > max_size) {
            enqcond.wait(mutex);
        }
        queue.push(std::move(t));
        
        if (guards) {
            Guard *guard = guards->guard;
            Lock guardlock(guard->mutex);
            guard->value = true;
            guardlock.unlock();
            guard->cond.signal();
        }
    }
    deqcond.signal();
}

template <typename T>
T Chan<T>::recv() {
    Lock lock(mutex);
    
    while (queue.empty()) {
        if (closed)
            return T();
        
        deqcond.wait(mutex);
    }
    
    T item = queue.front();
    size oldqueuesize = queue.size();
    queue.pop();
    
    lock.unlock();
    
    if (oldqueuesize == max_size) {
        enqcond.signal();
    }
    
    return item;
}
template <typename T>
bool Chan<T>::recv(T& t) {
    Lock lock(mutex);
    
    while (queue.empty()) {
        if (closed)
            return false;
        
        deqcond.wait(mutex);
    }
    
    t = queue.front();
    queue.pop();
    size queuesize = queue.size();
    
    lock.unlock();
    
    if (queuesize >= max_size)
        enqcond.signal();
    
    return true;
}

template <typename T>
bool Chan<T>::poll(T& item) {
    Lock lock(mutex);
    if (queue.empty())
        return false;
    
    item = queue.front();
    queue.pop();
    size queuesize = queue.size();
    
    lock.unlock();
    
    if (queuesize >= max_size)
        enqcond.signal();
    
    return true;
}

template <typename T>
void Chan<T>::close() {
    Lock lock(mutex);
    closed = true;
    lock.unlock();
    deqcond.broadcast();
}

template <typename T, typename... Args>
bool wait(Guard *guard, Chan<T>& chan) {
    Lock lock(chan.mutex);
    
    if (!chan.queue.empty())
        return true;
    
    if (chan.closed)
        return false;
    
    GuardList curr {guard, nullptr, chan.guards};
    if (chan.guards)
        chan.guards->prev = &curr;
    chan.guards = &curr;
    
    lock.unlock();
    
    {
        Lock guardlock(guard->mutex);
        while (!guard->value) 
            guard->cond.wait(guard->mutex);
    }
    
    lock.relock();
    
    if (curr.prev)
        curr.prev->next = curr.next;
    else
        chan.guards = curr.next;
    if (curr.next)
        curr.next->prev = curr.prev;
    
    return true;
}

template <typename T, typename... Args>
bool wait(Guard *guard, Chan<T>& chan, Args&... args) {
    Lock lock(chan.mutex);

    if (!chan.queue.empty())
        return true;
    
    if (chan.closed)
        return wait(guard, args...);
        
    GuardList curr {guard, nullptr, chan.guards};
    if (chan.guards)
        chan.guards->prev = &curr;
    chan.guards = &curr;
    
    lock.unlock();
    wait(guard, args...);
    lock.relock();
    
    if (curr.prev)
        curr.prev->next = curr.next;
    else
        chan.guards = curr.next;
    if (curr.next)
        curr.next->prev = curr.prev;
    
    return true;
}

template <typename T, typename... Args>
bool wait(Chan<T>& chan, Args&... args) {
    Guard guard;
    return wait(&guard, chan, args...);
}

namespace internal {
template <typename RetType>
bool select(sync::Guard *guard, bool success, RetType& retback, RetType ret) {
    if (!success) {
        retback = ret;
        return true;
    }
    
    sync::Lock guradlock(guard->mutex);
    while (!guard->value)
        guard->cond.wait(guard->mutex);
    
    return false;
}
template <typename RetType, typename T, typename... Args>
bool select(sync::Guard *guard, bool success, RetType& retback, RetType ret, sync::Chan<T>& chan, T& item, Args&&... args) {
    sync::Lock lock(chan.mutex);
    
    if (!chan.queue.empty()) {
        item = chan.queue.front();
        chan.queue.pop();
        size queuesize = chan.queue.size();
        
        lock.unlock();
        
        if (queuesize >= chan.max_size)
            chan.enqcond.signal();
        
        retback = ret;
        return true;
    }
    
    if (chan.closed) {
        return internal::select(guard, success, retback, args...);
    }
    
    sync::GuardList curr {guard, nullptr, chan.guards};
    if (chan.guards)
        chan.guards->prev = &curr;
    chan.guards = &curr;
    
    lock.unlock();
    bool b = internal::select(guard, true, retback, args...);
    lock.relock();
    
    if (curr.prev)
        curr.prev->next = curr.next;
    else
        chan.guards = curr.next;
    if (curr.next)
        curr.next->prev = curr.prev;
    
    if (!b && !chan.queue.empty()) {
        item = chan.queue.front();
        chan.queue.pop();
        size queuesize = chan.queue.size();
        
        lock.unlock();
        
        if (queuesize >= chan.max_size)
            chan.enqcond.signal();
        
        retback = ret;
        return true;
    }
    
    return b;
}
}

template <typename RetType, typename T, typename... Args>
RetType select(RetType ret, sync::Chan<T>& chan, T& item, Args&&... args) {
    RetType retback;
    sync::Guard guard;
    while (!internal::select(&guard, false, retback, ret, chan, item, args...)) {
        print "repeat";
    }
    
    return retback;
}

}}
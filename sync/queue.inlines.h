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
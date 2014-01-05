template <typename T, typename... Args>
T* Allocator::make(Args&&... args) {
    T *t = (T*) region.alloc(sizeof(T), alignof(T));
    new (t) T(std::forward<Args>(args)...);
    return t;
}

template <typename T, typename... Args>
T* Allocator::make_many(size count, Args&&... args) {
    T *first = (T*) region.alloc(sizeof(T) * count, alignof(T));
    T *curr = first;
    T *last = first+count;
    
    while (curr != last) {
        new (curr) T(std::forward<Args>(args)...);
        curr++;
    }
    
    return first;
}

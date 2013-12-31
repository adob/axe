template <typename T, typename... Args>
T& Allocator::make(Args&&... args) {
    T *t = (T*) region.alloc(sizeof(T), alignof(T));
    new (t) T(std::forward<Args>(args)...);
    return *t;
}
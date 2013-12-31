
// array //
template <typename T>
array<T>::array() 
  : len(0) 
{
    // empty
}

template <typename T> 
template <size N>
constexpr array<T>::array(T (&arr)[N])
  : arr(arr)
  , len(N)
{
    // empty
}

template <typename T>
constexpr const T& array<T>::operator [] (size i) {
    return assert(i < len),
           arr[i];
}

template <typename T>
size len(array<T> arr) {
    return arr.len;
}

// Array //
template <typename T, size N>
constexpr size len(Array<T, N> const&) {
    return N;
}

// Vector //
template <typename T>
Vector<T>::Vector(Allocator& alloc) 
  : len(0)
  , cap(0)
  , alloc(alloc) 
{
    // empty
}

// SmallVector //
template <typename T, size N>
SmallVector<T, N>::SmallVector(Allocator& alloc)
  : arr(buf)
  , len(0)
  , cap(N)
  , alloc(alloc)
{
    // empty
}

template <typename T>
List<T>::List(Allocator& alloc)
  : head(nullptr)
  , tail(nullptr)
  , alloc(alloc)
{
    // empty
}

template <typename T>
List<T>::operator list<T> () {
    return list<T>{head};
}

template <typename T>
template <typename... Args>
T* List<T>::make(Args&&... args) {
    typename list<T>::Node *node = &alloc.make<typename list<T>::Node>(std::forward<Args>(args)...);
    node->next = nullptr;
    
    if (tail) {
        tail->next = node;
    }
    if (!head) {
        head = node;
    }
    tail = node;
    return &node->data;
}




// template <typename T>
// constexpr T& array<T>::operator [] (size i) {
//     return assert(i < len),
//            arr[i];
// }





// Vector //
template <typename T>
Vector<T>::Vector(Allocator& alloc) 
  : data(nullptr)
  , len(0)
  , cap(0)
  , alloc(alloc) 
{
    // empty
}
template <typename T>
Vector<T>::Vector(size count, Allocator& alloc) 
: len(count)
, cap(count)
, alloc(alloc) 
{
    data = alloc.make_many<T>(count);
}
template <typename T>
size len(Vector<T> const& vec) {
    return vec.len;
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
  , len(0)
  , alloc(alloc)
{
    // empty
}

template <typename T>
List<T>::operator list<T> () {
    return list<T>{head, len};
}

template <typename T>
template <typename... Args>
T* List<T>::make(Args&&... args) {
    
    Node<T> *node = alloc.make< Node<T> >(std::forward<Args>(args)...);
    
    node->next = nullptr;
    
    if (tail) {
        tail->next = node;
    }
    if (!head) {
        head = node;
    }
    tail = node;
    len++;
    return &node->data;
}

template <typename T>
template <typename U, typename... Args>
U* List<T>::make(Args&&... args) {
    Node<U> *node = alloc.make< Node<U> >(std::forward<Args>(args)...);
    
    node->next = nullptr;
    
    if (tail) {
        tail->next = node;
    }
    if (!head) {
        head = node;
    }
    tail = node;
    len++;
    return &node->data;
}
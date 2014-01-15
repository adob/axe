#import "alloc.h"

namespace axe {

template <typename T> 
struct array {
    T       *arr;
    size     len;
    
    
    constexpr array() : arr(nullptr), len(0) {}
    constexpr array(T *t, usize len) : arr(t), len(len) {}
    
    template <size N> 
    constexpr array(T (&)[N])  : arr(arr), len(N) { }
    
    template <size N> 
    constexpr array(T (&&)[N]) : arr(arr), len(N) { }
    
    array slice(usize i) const {
        return assert(i <= len, exception::BadIndex, i, len),
               array(arr+i, len-i);
    }
    
    array slice(usize i, usize j) const {
        return assert(i <= len, exception::BadIndex, i , len),
               assert(i <= j, exception::BadIndex, i, j),
               array(arr+i, j-i);
    }
    
    array operator () (usize i) const {
        return slice(i);
    }
    
    array operator () (usize i, usize j) const {
        return slice(i, j);
    }
    
    T& operator [] (usize i) {
        return assert(i < len, exception::BadIndex, i, len-1), 
               arr[i];
    }
    constexpr const T& operator [] (usize i) const {
        return assert(i < len, exception::BadIndex, i, len-1), 
               arr[i];
    }
};

template <typename T>
size len(array<T> arr) {
    return arr.len;
}

template <typename T>
constexpr T *begin(array<T> arr) { return arr.arr; }

template <typename T>
constexpr T *end(array<T> arr) { return arr.arr + arr.len; }

template <typename T> size len(array<T> arr);


template <typename T, size N>
struct Array {
    T data[N];
    
//     Array() = default;
//     Array(const T (&arr)[N]) {
//         memcpy(data, arr, N * sizeof(T));
//     }
    //Array(std::initializer_list<T>) = default;
    
    Array<T, N>& operator = (const T (&arr)[N]) {
        memcpy(data, arr, N * sizeof(T));
        return *this;
    }
    
    T& operator [] (size n) {
        return assert(n < N, exception::BadIndex, n, N-1),
               data[n];
    }
    
    T const& operator [] (size n) const {
        return assert(n < N, exception::BadIndex, n, N-1),
               data[n];
    }
    
    operator bufref () {
        return bufref(data, N);
    }
    
    operator strref () const {
        return strref(data, N);
    }
} ;

template <typename T, size N>
constexpr size len(Array<T, N> const&) {
    return N;
}

template <typename T>
struct Vector {
    T         *data;
    size       len;
    size       cap;
    Allocator& alloc;
    
    Vector(Allocator& alloc);
    Vector(size count, Allocator &alloc);
    
    operator T* () {
        return data;
    }
    
    T* begin() {
        return data;
    }
    
    T* end() {
        return data+len;
    }
} ;

template <typename T>
size len(Vector<T> const&);

template <typename T, size N>
struct SmallVector {
    T         *arr;
    size       len;
    size       cap;
    Allocator& alloc;
    T          buf[N];
    
    SmallVector(Allocator& alloc);
} ;

template <typename>
struct Node;

template <>
struct Node<void> {
    Node<void> *next;
} ;

template <typename T>
struct Node : Node<void> {
    T     data;
} ;

template <typename T>
struct list {
    
    Node<void> *head;
    size        len;
    
    struct Iterator {
        Node<void> *curr;
        
        T&   operator *  () { return ((Node<T>*) curr)->data; }
        void operator ++ () { curr = curr->next; }
        bool operator != (Iterator other) { return curr != other.curr; }
    } ;
    
    list()                 : head(nullptr), len(0) {}
    list(Node<void> *head, size len) : head(head), len(len)    {}
    
    Iterator begin() {
        return Iterator{head};
    }
    
    Iterator end() {
        return Iterator{nullptr};
    }
    
    T& first() { return ((Node<T>*) head)->data; }
} ;


template <typename T>
size len(list<T> l) {
    return l.len;
}
template <typename T>
bool empty(list<T> l) {
    return l.len == 0;
}
template <typename T>
T& first(list<T> l) {
    return l.first();
}


template <typename T>
struct List {
    Node<void>        *head;
    Node<void>        *tail;
    size               len;
    Allocator&         alloc;
    
    List(Allocator&);
    
    template <typename... Args>
    T* make(Args&&... args);
    
    template <typename U, typename... Args>
    U* make(Args&&... args);
    
    operator list<T> ();
    
} ;

// template <typename T>
// struct list {
//     struct Node {
//         T     data;
//         Node *next;
//         Node *prev;
//     } ;
//     
//     Node *head;
//     Node *tail;
//     
//     list()                       : head(nullptr), tail(nullptr) {}
//     list(Node *head, Node *tail) : head(head),    tail(tail) {}
// } ;

#include "adt.inlines.h"
}
#import "alloc.h"

namespace axe {

template <typename T> 
struct array {
    T       *arr;
    size     len;
    
    array();
    
    template <size N> 
    constexpr array(T (&)[N]);
    
    template <size N> 
    constexpr array(T (&&)[N]);
    
    //constexpr       T& operator [] (size i);
    constexpr const T& operator [] (size i) const;
};

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
        assert(n < N);
        return data[n];
    }
    
    T const& operator [] (size n) const {
        assert(n < N);
        return data[n];
    }
    
    operator bufref () {
        return bufref(data, N);
    }
    
    operator strref () const {
        return strref(data, N);
    }
} ;

template <typename T, size N>
constexpr size len(Array<T, N> const& arr);

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
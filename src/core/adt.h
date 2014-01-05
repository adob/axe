#pragma once
#include <axe/core/alloc.h>

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
    
    Array() = default;
    Array(const T (&arr)[N]) {
        memcpy(data, arr, N * sizeof(T));
    }
    
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



template <typename T>
struct list {
    struct Node {
        T     data;
        Node *next;
    } ;
    
    Node *head;
    
    struct Iterator {
        Node *curr;
        
        T&   operator *  () { return curr->data; }
        void operator ++ () { curr = curr->next; }
        bool operator != (Iterator other) { return curr != other.curr; }
    } ;
    
    list()           : head(nullptr) {}
    list(Node *head) : head(head)    {}
    
    Iterator begin() {
        return Iterator{head};
    }
    
    Iterator end() {
        return Iterator{nullptr};
    }
} ;

template <typename T>
struct List {
    typename list<T>::Node     *head;
    typename list<T>::Node     *tail;
    Allocator&                  alloc;
    
    List(Allocator&);
    
    template <typename... Args>
    T* make(Args&&... args);
    
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
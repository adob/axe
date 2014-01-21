#include <utility>

#import <axe/typedefs.h>
#import <axe/alloc.h>
namespace axe {
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
    struct List {
        Node<void>        *head;
        Node<void>        *tail;
        size               len;
        Allocator&         alloc;
        
        List(Allocator& alloc)
          : head(nullptr)
          , tail(nullptr)
          , len(0)
          , alloc(alloc)
        {
            // empty
        }

        
        template <typename... Args>
        T* add(Args&&... args)  {
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
        
        template <typename U, typename... Args>
        U* add(Args&&... args) {
            
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
        
        operator list<T> () {
            return list<T>{head, len};
        }
        
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
    
    
}
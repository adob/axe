#include <string>
#include <memory>

#import <axe/core.h>

namespace axe {
    namespace sync {
        template <typename T>
        struct ARC {
            std::shared_ptr<T> ptr;
            
            ARC() {}
            ARC(ARC & other) : ptr(other.ptr) {}
            ARC(ARC const& other) : ptr(other.ptr) {}
            ARC(ARC && other) : ptr(std::move(other.ptr)) {}
            
            template <typename... Args>
            ARC(Args&&... args) : ptr(std::make_shared<T>(std::forward<Args>(args)...)) {}
            
            
            ARC& operator = (ARC & other) {
                ptr = other.ptr;
                return *this;
            }
            
            ARC& operator = (ARC const& other) {
                ptr = other.ptr;
                return *this;
            }
            
            ARC& operator = (ARC&& other) {
                ptr = std::move(other.ptr);
                return *this;
            }
            
            template <typename U>
            ARC& operator = (U&& other) {
                *ptr = std::forward<U>(other);
                return *this;
            }
            
            operator T& () const {
                return *ptr;
            }
            
            T* operator -> () {
                return &(*ptr);
            }
            
            const T* operator -> () const {
                return &(*ptr);
            }
            
            T& operator * () {
                return *ptr;
            }
            
            const T& operator * () const {
                return *ptr;
            }
            
            str string(Allocator& alloc) const {
                Str s(alloc);
                s += "ARC<";
                s += ptr->string(alloc);
                s += ">";
                return s;
            }
            
            friend uintptr hash(ARC const& t) {
                return hash(*t);
            }
        };
        
        template <typename T>
        bool operator == (ARC<T> const& left, ARC<T> const& right) {
            return *left.ptr == *right.ptr;
        }
        
        template <typename T>
        bool operator != (ARC<T> const& left, ARC<T> const& right) {
            return *left.ptr != *right.ptr;
        }
    }
}
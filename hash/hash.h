#include <type_traits>
#import <axe/core.h>
#import "city.h"

namespace axe {
    
    static_assert(sizeof(uintptr) == 8, "size mismatch");
    
    inline uintptr hash(const char *data, size len) {
        return CityHash64(data, len);
    }
    
    inline uintptr hash(str s) {
        return hash(s.data, s.len);
    }
    
    inline uintptr hash(void *ptr) {
        return CityHash64((const char *)&ptr, sizeof ptr);
    }
    
    struct Hash {
        
        template <typename T>
        uintptr operator () (T const& t) const {
            return hash(t);
        }
    } ;
    
    template <typename T>
    struct Hashed {
        T       data;
        uintptr code;
        
        Hashed(Hashed & other) : data(other.data), code(other.code) {}
        Hashed(Hashed const& other) : data(other.data), code(other.code) {}
        Hashed(Hashed && other) : data(std::move(other.data)), code(other.code) {}
        
        template <typename... Args>
        Hashed(Args&&... args)
          : data(std::forward<Args>(args)...), code(hash(data)) {}
        
        Hashed& operator = (Hashed & other) {
            data = other.data;
            code = other.code;
            return *this;
        }
        
        Hashed& operator = (Hashed const& other) {
            data = other.data;
            code = other.code;
            return *this;
        }
        
        Hashed& operator = (Hashed&& other) {
            data = std::move(other.data);
            code = other.code;
            return *this;
        }
        
        template <typename U>
        Hashed& operator = (U&& other) {
            data = other;
            code = hash(data);
            return *this;
        }
        
        operator T& () const {
            return data;
        }
        
//         template <typename U, size V = sizeof( (U) data ) > 
//         operator U () const {
//             return data;
//         }
        
        T* operator -> () {
            return &data;
        }
        
        const T* operator -> () const {
            return &data;
        }
        
        T& operator * () {
            return data;
        }
        
        const T& operator * () const {
            return data;
        }
        
        bool operator == (Hashed<T> const& other) const {
            return data == other.data;
        }
        
        bool operator != (Hashed<T> const& other) const {
            return data != other.data;
        }
        
        str string(Allocator& alloc) const {
            Str s(alloc);
            s += "Hashed<";
            s += data.string(alloc);
            s += ">";
            return s;
        }
        
        //template <typename T>
        friend uintptr hash(Hashed<T> const& t) {
            return t.code;
        }
    } ;
    

    

    
}
#import <axe/exceptions.h>
#import <axe/assert.h>
namespace axe {
    template <typename T> 
    struct array {
        T       *arr;
        size     len;
        
        
        constexpr array() : arr(nullptr), len(0) {}
        constexpr array(T *t, usize len) : arr(t), len(len) {}
        
        template <size N> 
        constexpr array(T (&arr)[N])  : arr(arr), len(N) { }
        
        template <size N> 
        constexpr array(T (&&arr)[N]) : arr(arr), len(N) { }
        
        array slice(usize i) const {
            return assert(i <= len, exceptions::BadIndex, i, len),
            array(arr+i, len-i);
        }
        
        array slice(usize i, usize j) const {
            return assert(i <= len, exceptions::BadIndex, i , len),
            assert(i <= j, exceptions::BadIndex, i, j),
            array(arr+i, j-i);
        }
        
        array operator () (usize i) const {
            return slice(i);
        }
        
        array operator () (usize i, usize j) const {
            return slice(i, j);
        }
        
        T& operator [] (usize i) {
            return assert(i < len, exceptions::BadIndex, i, len-1), 
            arr[i];
        }
        constexpr const T& operator [] (usize i) const {
            return assert(i < len, exceptions::BadIndex, i, len-1), 
            arr[i];
        }
        
        friend size len(array arr) {
            return arr.len;
        }
        
        friend constexpr T *begin(array arr) { return arr.arr; }
        
        friend constexpr T *end(array arr) { return arr.arr + arr.len; }
    };
    

}
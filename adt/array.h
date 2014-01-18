#import <axe/exceptions.h>
namespace axe {
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
            return assert(n < N, exceptions::BadIndex, n, N-1),
            data[n];
        }
        
        T const& operator [] (size n) const {
            return assert(n < N, exceptions::BadIndex, n, N-1),
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
}
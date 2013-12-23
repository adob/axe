#pragma once
#include <new>

namespace detail {
    template <typename R, typename... ArgTypes>
    struct funcref_interface {
        virtual R operator()(ArgTypes&&...) = 0;
    };
    
    template <typename T, typename R, typename... ArgTypes>
    struct funcref_data : funcref_interface<R, ArgTypes...> {
        T & t;
        
        funcref_data(T & t) : t(t) {}
        
        R operator()(ArgTypes&&... args) {
            return t(std::forward<ArgTypes>(args)...);
        }
    };
}

template <typename R=void, typename... ArgTypes>
struct funcref {
    
    union {
        char data[ 2 * sizeof(void*)  ];
        detail::funcref_interface<R, ArgTypes...> *func;
    };
    
    template <typename T>
    funcref(T&& t) {
        static_assert(sizeof(detail::funcref_data<T, R, ArgTypes...>) == sizeof(data), "size mismatch");
        new (data) detail::funcref_data<T, R, ArgTypes...>(t);
    }
    
    R operator()(ArgTypes&&... args) {
        return ((detail::funcref_interface<R, ArgTypes...> *) &func)->operator()(std::forward<ArgTypes>(args)...);
    }
};

// template <typename T, typename R=void, typename... Args>
// struct methodref {
//     using method_t = R (T::*)(Args...);
//     
//     T& t;
//     const method_t memberp;
//     
//     methodref(T& t, method_t memberp) : t(t), memberp(memberp) {}
//     
//     R operator ()(Args... args) const {
//         return (t.*memberp)(args...);
//     }
// };
// 
// template <typename T, typename R=void, typename... Args>
// methodref<T, R, Args...> 
// methbind(T& t, R (T::* memberp)(Args...)) {
//     return methodref<T, R, Args...>(t, memberp);
// }

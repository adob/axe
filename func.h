#include <new>
#include <type_traits>
#include <utility>
#include <stdlib.h>
#include <stdio.h>

#import <axe/typedefs.h>

namespace axe {
    namespace internal {
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
            internal::funcref_interface<R, ArgTypes...> *func;
        };
        
        template <typename T>
        funcref(T&& t) {
            static_assert(sizeof(internal::funcref_data<T, R, ArgTypes...>) == sizeof(data), "size mismatch");
            new (data) internal::funcref_data<T, R, ArgTypes...>(t);
        }
        
        R operator()(ArgTypes&&... args) {
            return ((internal::funcref_interface<R, ArgTypes...> *) &func)->operator()(std::forward<ArgTypes>(args)...);
        }
    };
    
    
    namespace internal {
        
        template <typename Ret, typename... Args>
        struct FuncIFace {
            virtual Ret operator () (Args&&... args) = 0;
            virtual ~FuncIFace() {}
        } ;
        
        template <typename Impl, typename Ret, typename... Args>
        struct FuncImpl : FuncIFace<Ret, Args...> {
            Impl impl;
            
            FuncImpl(Impl&& impl) : impl(std::forward<Impl>(impl)) {}
            
            virtual Ret operator () (Args&&... args) override {
                return impl(std::forward<Args>(args)...);
            }
            
            virtual ~FuncImpl() override {
                //printf("destructing func %p\n", this);
            }
        } ;
    }
    
    
    template <typename T>
    struct Func {
        
    } ;
    
    template <typename Ret, typename... Args>
    struct Func<Ret(Args...)> {
        internal::FuncIFace<Ret, Args...> *funcp;
        size buffer[7];
        
        Func()
          : funcp(nullptr)
          {}
        
        Func(Func<Ret(Args...)> const&) { 
            throw "Unimplemented"; 
        }
        
        Func(Func<Ret(Args...)>&& other) {
            if (((void*)other.funcp) == ((void*)other.buffer)) {
                memcpy(buffer, other.buffer, sizeof(buffer));
                funcp = (internal::FuncIFace<Ret, Args...> *) buffer;
            } else {
                funcp = other.funcp;
            }
            other.funcp = nullptr;
        }
        
        template <typename Functor>
        Func(Functor&& functor) {
            if (sizeof(functor) <= sizeof(buffer) && alignof(Functor) <= alignof(decltype(buffer))) {
                funcp = (internal::FuncIFace<Ret, Args...> *) buffer;
            } else {
                printf("alignof in funch.h\n");
                funcp = (internal::FuncIFace<Ret, Args...> *) malloc(sizeof(functor));
            }
            
            new (funcp) internal::FuncImpl<Functor, Ret, Args...>(std::forward<Functor>(functor));
        }
        
        Ret operator () (Args&&... args) {
            return (*funcp)(std::forward<Args>(args)...);
        }
        
        Func& operator = (std::nullptr_t) {
            if (funcp) {
                funcp->~FuncIFace();
                
                if (((void*)funcp) != ((void*)buffer)) {
                    free(funcp);
                }
            }
            funcp = nullptr;
            return *this;
        }
        
        Func& operator = (Func&& other) {
            if (funcp) {
                funcp->~FuncIFace();
                
                if (((void*)funcp) != ((void*)buffer)) {
                    free(funcp);
                }
            }
            if (((void*)other.funcp) == ((void*)other.buffer)) {
                memcpy(buffer, other.buffer, sizeof(buffer));
                funcp = (internal::FuncIFace<Ret, Args...> *) buffer;
            } else {
                funcp = other.funcp;
            }
            other.funcp = nullptr;
            return *this;
        }
        
        ~Func() {
            if (funcp) {
                funcp->~FuncIFace();
                
                if (((void*)funcp) != ((void*)buffer)) {
                    free(funcp);
                }
            }
        }
    } ;

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
}
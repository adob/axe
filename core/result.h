#pragma once
#include <axe/core/typedefs.h>
#include <axe/print.h>

namespace axe {
    namespace internal  {
        template <size index, typename... Args> struct result;
        
        template <size i, typename ArgFirst, typename... ArgRest> 
        ArgFirst& get(result<i, ArgFirst, ArgRest...> & r);
        
        template <size index>
        struct result<index> {
            
        } ;
        
        
        template <size index, typename ArgFirst, typename... ArgRest>
        struct result<index, ArgFirst, ArgRest...> : result<index+1, ArgRest...> {
            ArgFirst first;
            
            result(ArgFirst&& first, ArgRest... rest) 
            : result<index+1, ArgRest...>(rest...)
            ,  first(std::forward<ArgFirst>(first))
            {}
            
            template <size i>
            auto get() -> decltype(get<i>(*this)) {
                return internal::get<i>(*this);
            }
            
        } ;
        
        template <size i, typename ArgFirst, typename... ArgRest>
        ArgFirst& get(result<i, ArgFirst, ArgRest...> & r) {
            ArgFirst& ref = r.first;
            return ref;
        }
    }

    template <typename ArgFirst, typename... ArgRest>
    struct result : internal::result<0, ArgFirst, ArgRest...> {
        result(ArgFirst&& first, ArgRest... rest) 
        : internal::result<0, ArgFirst, ArgRest...>(std::forward<ArgFirst>(first), rest...) {
            print "result constructor1";
        }
        
        result(ArgFirst const& first, ArgRest... rest) 
        : internal::result<0, ArgFirst, ArgRest...>(first, rest...) {
            print "result constructor2";
        }
    } ;
}
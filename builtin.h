#pragma once

#include "typedefs.h"
#include "str.h"
#include "error.h"
#include "allocator.h"
using str     = strref;
using ByteBuf = Buf;
using StrBuf  = Buf;
using Str     = Buf;
using byteref = bufref;

//extern char __thread thread_local_buf[1024*8];
const size npos = (size) -1;
const nullptr_t nil = nullptr;

namespace helper  {
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
            return helper::get<i>(*this);
        }
        
    } ;
    
    template <size i, typename ArgFirst, typename... ArgRest>
    ArgFirst& get(result<i, ArgFirst, ArgRest...> & r) {
        ArgFirst& ref = r.first;
        return ref;
    }
}

template <typename ArgFirst, typename... ArgRest>
struct result : helper::result<0, ArgFirst, ArgRest...> {
    result(ArgFirst&& first, ArgRest... rest) 
      : helper::result<0, ArgFirst, ArgRest...>(std::forward<ArgFirst>(first), rest...) {
        print "result constructor1";
    }
    
    result(ArgFirst const& first, ArgRest... rest) 
    : helper::result<0, ArgFirst, ArgRest...>(first, rest...) {
        print "result constructor2";
    }
} ;
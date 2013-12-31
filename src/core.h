#pragma once

#include <axe/core/typedefs.h>
#include <axe/core/str.h>
#include <axe/core/error.h>
#include <axe/core/alloc.h>
#include <axe/core/adt.h>

namespace axe {

    template <typename Func>
    struct Deferred {
        Func func;
        
        ~Deferred() {
            func();
        }
    };

    template <typename Func>
    Deferred<Func> defer(Func&& func) {
        return Deferred<Func>{std::move(func)};
    }

    esize write(FILE*, str);

    struct Writer {
        template <typename T>
        esize operator () (T&& thing) {
            return write(stdout, std::forward<T>(thing));
        }
    } ;

    inline esize write(Writer& writer, str s) {
        return writer(s);
    }
}
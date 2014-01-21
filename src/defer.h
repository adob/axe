#import <utility>
#import "func.h"
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
        return Deferred<Func>{std::forward<Func>(func)};
    }
}
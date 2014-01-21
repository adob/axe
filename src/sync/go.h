#import <axe/func.h>
#import <axe/errorparam.h>
namespace axe {
    namespace sync {
        void init(errorparam err = {});
        void go(Func<void()> const& f);
        void go(Func<void()>     && f);
    }
}
#include <axe/core.h>
#include <functional>

namespace axe {
namespace csp {
    void init(errorparam err = {});
    
    using functype = std::function<void()>;
    
    void go(functype f);
}
}
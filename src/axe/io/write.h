#import <axe/str.h>
#import <axe/errorparam.h>

namespace axe {
    namespace io {
        size write(FILE *f, str s, errorparam = {});
        size write(int fd, str s, errorparam = {});
        
        template <typename Writer>
        size write(Writer&& w, str s, errorparam err) {
            return w.write(s, err);
        }
        
        template <typename Writer>
        size write(Writer&& w, str s) {
            return w.write(s);
        }
        
    }
}
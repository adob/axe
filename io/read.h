#include <stdio.h>
#import <axe/str.h>
#import <axe/errorparam.h>
namespace axe {
    namespace io {
        size read(FILE *, buf b, errorparam  = {});
        size read(int fd, buf b, errorparam = {});
        
        template <typename Reader>
        size read(Reader&& r, buf b, errorparam err) {
            return r.read(b, err);
        }
        
        template <typename Reader>
        size read_full(Reader&& reader, buf buffer, errorparam err = {}) {
            size n = 0;
            size min = len(buffer);
            do {
                size nn = read(reader, buffer(n), err);
                if (err) {
                    return n+nn;
                } else if (nn == 0) {
                    err = EOF;
                    return n;
                }
                n += nn;
            } while (n < min);
            return n;
        }
    }
}
#include <unistd.h>
#import "write.h"

namespace axe { 
    namespace io {
        
        size write(FILE *f, str s, errorparam err) {
            size n = fwrite(s.data, 1, s.len, f);
            if (n != len(s)) {
                if (feof(f)) {
                    return err=EOF, n;
                } else {
                    return err=errno, n;
                }
            }
            return n;
        }
        
        size write(int fd, str s, errorparam err) {
            size ret = ::write(fd, s.data, s.len);
            if (ret == -1) {
                return err=errno, 0;
            } else if (ret < len(s)) {
                return err=EOF, ret;
            }
            return ret;
        }
    }
}
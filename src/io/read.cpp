#include <unistd.h>
#import "read.h"

namespace axe { 
    namespace io {
        size read(FILE *file, buf b, errorparam err) {
            //printf("fread");
            size n = fread(b.data, 1, len(b), file);
            if (n != len(b)) {
                if (feof(file)) {
                    return n;
                } else {
                    return err=errno, n;
                }
            }
            return n;
        }
        
        size read(int fd, buf b, errorparam err) {
            size ret = ::read(fd, b.data, b.len);
            if (ret == -1) {
                return err=errno, 0;
            }
            return ret;
        }
    }
    
}
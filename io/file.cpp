#include <unistd.h>
#import <axe/raise.h>
#import "file.h"

namespace axe {
    namespace io {
        File& File::operator = (File&& other) {
            if (fd != -1)
                close();
            fd = other.fd;
            other.fd = -1;
            return *this;
        }
        
        File::~File() {
            if (fd != -1) {
                int ret = ::close(fd);
                if (ret) {
                    raise(errno);
                }
            }
        }
        
        void File::close(errorparam err) {
            if (fd == -1) {
                return err="Already closed";
            }
            
            int ret = ::close(fd);
            fd = -1;
            if (ret) {
                return err=errno;
            }
        }
        
    }
}
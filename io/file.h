#import <axe/errorparam.h>

namespace axe {
    namespace io {
        struct File {
            int fd = -1;
            
            File() = default;
            explicit File(int fd) : fd(fd) {}
            File(File const&) = delete;
            File(File&& other) : fd(other.fd) { other.fd = -1; }
            
            File& operator = (File const&) = delete;
            File& operator = (File&& other);
            
            void close(errorparam err = {});
            
            ~File();
        };
    }
}
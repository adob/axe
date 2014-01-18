#import <axe/errorparam.h>
#import <axe/buffer.h>
namespace axe {
    namespace io {
        size write(FILE *f, str s, errorparam = {});
        size write(int fd, str s, errorparam = {});
        
        inline void write(Buffer& b, str s, errorparam = {}) {
            b.append(s);
        }
        
        size read(FILE *, buf b, errorparam  = {});
        size read(int fd, buf b, errorparam = {});
        
        template <typename Writer>
        size write(Writer&& w, str s, errorparam err) {
            return w.write(s, err);
        }
        
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
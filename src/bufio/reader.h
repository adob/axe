#import <axe/io/read.h>
#import <axe/strings.h>
#import "bufio.h"
namespace axe { 
    namespace bufio {
        template <typename T>
        struct Reader {
            Buffer  buffer;
            T&      reader;
            size    pos;
            size    limit;
            
            Reader(T& reader, size bufsize = DefaultBufSize)
            : buffer(bufsize, bufsize)
            , reader(reader)
            , pos(0)
            , limit(0) {
                
            }
            
            size read(buf out, errorparam err = {}) {
                if (pos == limit) {
                    if (len(out) >= len(buffer)) {
                        return io::read(reader, out, err);
                    }
                    fill(err);
                }
                
                size n = std::min(len(out), limit - pos);
                memcpy(out.data, buffer.data+pos, n);
                pos += n;
                return n;
            }
            
            str read(char delim, errorparam err = {}) {
                size i = strings::index(buffer(pos, limit), delim);
                if (i != -1) {
                    str line = buffer(pos, pos+i+1);
                    pos += i + 1;
                    return line;
                }
                
                size n = buffered();
                for (;;) {
                    size m = fill(err);
                    if (err) {
                        str line = buffer(pos, pos + n + m);
                        pos += m;
                        return line;
                    }
                    if (m == 0) {
                        err = EOF;
                        return buffer(pos, pos + n);
                    }
                    
                    size i = strings::index(buffer(n, limit), delim);
                    if (i != -1) {
                        str line = buffer(pos, n + i + 1);
                        pos += n + i + 1;
                        return line;
                    }
                    
                    
                    if (buffered() >= len(buffer)) {
                        pos = limit;
                        return err=ErrBufferFull, buffer;
                    }
                    
                    n += m;
                }
            }
            
            //         friend size read(Reader<T>& r, buf b, errorparam err = {}) {
                //             return r.read(b, err);
                //         }
                
                byte read_byte(errorparam err = {}) {
                    if (pos == limit) {
                        size n = fill(err);
                        if (err) {
                            return 0;
                        } else if (n == 0) {
                            return err=EOF, 0;
                        }
                    }
                    return buffer[pos++];
                }
                
                size buffered() const {
                    return limit - pos;
                }
                
        private:
            size fill(errorparam err) {
                if (pos > 0) {
                    memmove(buffer.data, buffer.data+pos, limit-pos);
                    limit -= pos;
                    pos    = 0;
                }
                size n = io::read(reader, buffer(limit), err);
                limit += n;
                return n;
            }
        } ;
    }
}
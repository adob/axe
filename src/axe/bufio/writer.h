#import <axe/str.h>
#import <axe/buffer.h>
#import <axe/io/write.h>
#import "bufio.h"
namespace axe { 
    namespace bufio {
        
        template <typename T>
        struct Writer {
            Buffer buffer;
            T&     writer;
            size   pos;
            
            Writer(T& writer, size bufsize = DefaultBufSize)
              : buffer(bufsize, bufsize)
              , writer(writer)
              , pos(0) {
                
            }
            
            size write(str in, errorparam err = {}) {
                // if the buffer is empty and the data is large
                // just send the data directly
                if (len(in) >= len(buffer) && pos == 0) {
                    return io::write(writer, in, err);
                }
                
                // otherwise copy data into buffer
                size n = copy(buffer(pos), in);
                pos += n;
                
                // if the data was too big for the buffer
                // we have to flush the buffer
                if (n < len(in)) {
                    // flush the buffer up to pos
                    size m = io::write(writer, buffer(0, pos), err);
                    pos -= m;
                    if (pos > 0) {
                        // short write
                        copy(buffer, buffer(m, m + pos));
                        return n;
                    }
                    
                    // if the data that's left is larger than buffer
                    // just send the data directly
                    size left = len(in) - n;
                    if (left >= len(buffer)) {
                        return n + io::write(writer, in, err);
                    } else {
                        // otherwise just copy it
                        return n + copy(buffer, in(n));
                    }
                }
                
                return n;
            }
            
            void write(byte b, errorparam err = {}) {
                if (available() == 0) {
                    size n = flush(err);
                    if (err) {
                        return;
                    } else if (n == 0) {
                        err = EOF;
                        return;
                    }
                }
                buffer[pos++] = b;
            }
            
    //         friend size write(Writer<T>& w, str s, errorparam err = {}) {
    //             return w.write(s, err);
    //         }
            
            size flush(errorparam err = {}) {
                size n = io::write(writer, buffer(0, pos), err);
                pos -= n;
                if (pos > 0) {
                    copy(buffer, buffer(n, n + pos));
                }
                return n;
            }
            
            size full_flush(errorparam err = {}) {
                size nn = 0;
                while (pos > 0) {
                    size n = flush(err);
                    nn += n;
                    if (err) {
                        return nn;
                    } else if (n == 0) {
                        err = EOF;
                        return nn;
                    }
                }
                return nn;
            }
            
            size available() {
                return len(buffer) - pos;
            }
            
        } ;
    }
}
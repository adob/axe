#import <axe/core.h>
#import <axe/io.h>

namespace axe { namespace bufio {
    
    const size DefaultBufSize = 4096;
    
    template <typename T>
    struct Reader {
        Buffer  buffer;
        T&      reader;
        size    pos;
        size    limit;
        
        Reader(T& reader, size bufsize = DefaultBufSize)
          : buffer(bufsize)
          , reader(reader)
          , pos(0)
          , limit(0) {
            
        }
        
        buf read(buf out, errorparam err = {}) {
            if (pos == limit) {
                if (len(out) >= len(buffer)) {
                    return io::read(reader, out, err);
                }
                fill(err);
            }

            size n = std::min(len(out), limit - pos);
            memcpy(out.data, buffer.data+pos, n);
            pos += n;
            return out(0, n);
        }
        
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
        
    private:
        size fill(errorparam err) {
            if (pos > 0) {
                memcpy(buffer.data, buffer.data+pos, limit-pos);
                limit -= pos;
                pos    = 0;
            }
            size n = io::read(reader, buffer(limit), err).len;
            limit += n;
            return n;
        }
    } ;
    
    template <typename T>
    struct Writer {
        Buffer buffer;
        T&     writer;
        size   pos;
        
        Writer(T& writer, size bufsize = DefaultBufSize)
          : buffer(bufsize)
          , writer(writer)
          , pos(0) {
            
        }
        
        str write(str in, errorparam err = {}) {
            
            while (len(in) > available()) {
                size n;
                if (pos == 0) {
                    n = io::write(writer, in, err).len;
                    if (err) {
                        return str();
                    } else if (n == 0) {
                        err = EOF;
                        return str();
                    }
                } else {
                    n = copy(buffer(pos), in);
                    pos += n;
                    flush(err);
                    if (err) {
                        return str();
                    }
                }
                in = in(n);
            }
            
            pos += copy(buffer(pos), in);
            return in;
        }
        
        void write(byte b, errorparam err = {}) {
            if (available() == 0) {
                flush(err);
                if (err) {
                    return;
                }
            }
            buffer[pos++] = b;
        }
        
        void flush(errorparam err = {}) {
            size nn = 0;
            size n;
            while (nn < pos) {
                n = io::write(writer, buffer(nn, pos), err).len;
                if (err) {
                    goto err;
                } else if (n == 0) {
                    err = EOF;
                    goto err;
                }
                nn += n;
            }
            pos = 0;
            return;
            
            err:
            copy(buffer, buffer(nn+n, pos));
            pos = nn + n;
            return;
        }
        
        size available() {
            return len(buffer) - pos;
        }
        
    } ;
}}
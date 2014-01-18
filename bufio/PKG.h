#import <axe/core.h>
#import <axe/io/PKG.h>
#import <axe/strings/PKG.h>

namespace axe { namespace bufio {
    
    const size DefaultBufSize = 4096;
    const error ErrBufferFull = "Buffer is full";
    
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
                memcpy(buffer.data, buffer.data+pos, limit-pos);
                limit -= pos;
                pos    = 0;
            }
            size n = io::read(reader, buffer(limit), err);
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
    
}}
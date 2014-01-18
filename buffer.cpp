#import "buffer.h"
#import <axe/utf8/utf8.h>
namespace axe {
    void Str::append(rune r) {
        ensure(utf8::UTFMax);
        len += axe::len(utf8::encode(r, buf(len)));
    }
    
    
    void Str::append_many(rune r, size count) {
        char minibuf[utf8::UTFMax];
        str out = utf8::encode(r, minibuf);
        for (size i = 0; i < count; i++) {
            memcpy(buf.data + len, out.data, out.len);
            len += out.len;
        }
    }
}
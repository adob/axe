#import "buffer.h"
#import <axe/unicode/utf8/utf8.h>
#import <axe/unicode/utf8/encode.h>
namespace axe {
    namespace utf8 = unicode::utf8;
    
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
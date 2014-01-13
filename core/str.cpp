#include <ostream>
#include <iostream>
#include <string.h>

#import <axe/utf8/PKG.h>
#import "PKG.h"


//#include "print.h"

namespace axe {

std::ostream& operator << (std::ostream & o, const strref str) {
    o.write(str.data, str.len);
    return o;
}


TempCStr strref::tmp_c_str() {
    return TempCStr(data, len);
}

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

TempCStr::TempCStr(const char *data, size len) {
    if (data == nullptr) {
        charp = nullptr;
        return;
    }

    if (len > 255) {
        charp = (char*) malloc(len+1);
    } else {
        charp = buf;
    }
    memcpy(charp, data, len);
    charp[len] = '\0';
}

TempCStr::operator const char * () {
    return charp;
}
    
TempCStr::~TempCStr() {
    if (charp != buf) {
        free(charp);
    }
}

}
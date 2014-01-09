#include <ostream>
#include <iostream>
#include <string.h>
#include "str.h"
#include "unicode/utf8.h"

//#include "print.h"

namespace axe {

std::ostream& operator << (std::ostream & o, const strref str) {
    o.write(str.data, str.len);
    return o;
}

bufref bufref::slice(size i) const { 
    assert(i < len); 
    return bufref(data+i, len-i); 
}

bufref bufref::slice(size i, size j) const { 
    assert(i < len && j <= len && i <= j); 
    return bufref(data+i, j-i); 
}

byte& bufref::operator [] (size i) const {
    assert(i < len);
    return data[i];
}

bufref bufref::operator()(size i, size j) const {
    return slice(i, j);
}

bufref bufref::operator()(size i) const  {
    return slice(i);
}

TempCStr strref::tmp_c_str() {
    return TempCStr(data, len);
}

strref::operator bool () {
    return data != nullptr;
}

size copy(bufref dst, strref src) {
    size n = src.len > dst.len ? dst.len : src.len;
    memmove(dst.data, src.data, n);
    return n;
}

Buf::Buf(Allocator& alloc, byte *buf, size capacity) 
  : buf(buf, capacity)
  , len(0)
  , alloc(alloc) 
{
    
}

Buf::Buf(Allocator& alloc, size capacity)
  : buf(alloc(capacity))
  , len(0)
  , alloc(alloc) 
{
    
}

void Buf::append(str s) {
    ensure(s.len);
    memmove(buf.data + len, s.data, s.len);
    len += s.len;
}

void Buf::append(char c) {
    ensure(1);
    buf[len] = c;
    len += 1;
}

void Buf::append(byte b) {
    append((char) b);
}

void Buf::append(rune r) {
    ensure(utf8::UTFMax);
    len += axe::len(utf8::encode(r, buf(len)));
}

void Buf::append_many(str s, size count) {
    ensure(s.len * count);
    for (size i = 0; i < count; i++) {
        memmove(buf.data + len, s.data, s.len);
        len += s.len;
    }
}

void Buf::append_many(char c, size count) {
    ensure(count);
    memset(buf.data + len, c, count);
    len += count;
}

void Buf::append_many(byte c, size count) {
    append_many((char) c, count);
}

void Buf::append_many(rune r, size count) {
    Array<char, utf8::UTFMax> minibuf;
    str out = utf8::encode(r, minibuf);
    for (size i = 0; i < count; i++) {
        memcpy(buf.data + len, out.data, out.len);
        len += out.len;
    }
}


void Buf::ensure(size cap) {
    if (len + cap > buf.len) {
        bufref newbuf = alloc(std::max(cap+buf.len, buf.len * 2));
        copy(newbuf, buf);
        buf = newbuf;
    }
}

Buf::operator bufref () {
    return buf(0, len);
}

Buf::operator strref () {
    return buf(0, len);
}

TempCStr::TempCStr(const char *data, size len) {
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

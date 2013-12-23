// #include <csignal>
// #include <execinfo.h>
// #include <unistd.h>
//#include <ostream>
// #include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include <tuple>
#include <utility>

#include "builtin.h"
#include "func.h"
#include "include/os"
#include "include/strings"
#include "include/unicode/utf8"

#include "print.h"

void foo(int32_t c) {
    print "int32";
}

void foo(uint32_t c) {
    print "uint32";
}

void foo(char32_t c) {
    print "char32";
}


int main() {
#if defined(__GXX_EXPERIMENTAL_CXX0X__ )
    print "char32_t defined";
#else
    print "char32_t not defined";
#endif
    print "Hello to test";
    str s = "é unicode str";
    
    int runesize;
    rune r = unicode::utf8::decode_rune(s, runesize);
    foo(r);
    print r;
    
    print "%q %q %q %q" % "abc", (char32_t) 233, (char32_t) 0xFFFFFFFF, (char32_t) '\\';
    
//     
//     r.first = 7;
//     get<0>(r) = 8;
//     get<1>(r) = 'c';
//     print get<0>(r), get<1>(r);
//     r.get<0>() = 9;
//     
//     print r.get<0>(), r.get<1>();
    //print "out", r.get<0>();
    
    
    //print fmt::sprintf(alloc, "foo");
    //int a, b;
    
    //tie(a, b) = foo();
    
    
//     for (uint32 c : s) {
//         print c;
//     }
//     str s2 = "abc";
    
    
    
//     http::handle("/bar", [](http::IResponseWriter w, HttpRequest& r) {
//         fmt::fprintf(w, "Hello") % html::escape(r.url.path);
//     });
    
    //MyStruct my_struct = make_mystruct();
    
//     fun1([](int i) {
//         print "called with %d" % i;
//     });
//     
//     MyStruct ms1(1);
//     
//     fun1( std::bind(&MyStruct::fun, &ms1, std::placeholders::_1) );
//     
    return 0;
}
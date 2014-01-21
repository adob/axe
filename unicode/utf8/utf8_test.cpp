#include <type_traits>
#import <axe.h>
#import <axe/print.h>
#import <axe/adt/map.h>
#import <axe/unicode/utf8.h>
#import <axe/debug.h>
#import <axe/fmt.h>
#import <axe/testing.h>

namespace axe {
    namespace unicode {
        namespace utf8 {
            struct UTF8Map {
                rune r;
                str  s;
            } ;
            
            UTF8Map utf8map[] = {
                {0x0000, str("\x00")},
                {0x0001, "\x01"},
                {0x007e, "\x7e"},
                {0x007f, "\x7f"},
                {0x0080, "\xc2\x80"},
                {0x0081, "\xc2\x81"},
                {0x00bf, "\xc2\xbf"},
                {0x00c0, "\xc3\x80"},
                {0x00c1, "\xc3\x81"},
                {0x00c8, "\xc3\x88"},
                {0x00d0, "\xc3\x90"},
                {0x00e0, "\xc3\xa0"},
                {0x00f0, "\xc3\xb0"},
                {0x00f8, "\xc3\xb8"},
                {0x00ff, "\xc3\xbf"},
                {0x0100, "\xc4\x80"},
                {0x07ff, "\xdf\xbf"},
                {0x0800, "\xe0\xa0\x80"},
                {0x0801, "\xe0\xa0\x81"},
                {0xd7ff, "\xed\x9f\xbf"}, // last code point before surrogate half.
                {0xe000, "\xee\x80\x80"}, // first code point after surrogate half.
                {0xfffe, "\xef\xbf\xbe"},
                {0xffff, "\xef\xbf\xbf"},
                {0x10000, "\xf0\x90\x80\x80"},
                {0x10001, "\xf0\x90\x80\x81"},
                {0x10fffe, "\xf4\x8f\xbf\xbe"},
                {0x10ffff, "\xf4\x8f\xbf\xbf"},
                {0xFFFD, "\xef\xbf\xbd"}
            };
            
            TESTCASE (full_rune) {
                for (const UTF8Map & m : utf8map) {
                    test(full_rune(m.s));
                    
                    str s1 = m.s(0, len(m.s)-1);
                    test(!full_rune(s1));
                }
            }
            
            TESTCASE (encode_rune) {
                for (const UTF8Map & m : utf8map) {
                    Array<byte, 10> buf;
                    str out = encode(m.r, buf);
                    
                    test(out == m.s);
                }
            }
            
            TESTCASE (decode_rune) {
                for (const UTF8Map & m : utf8map) {
                    int runesize;
                    rune r = utf8::decode(m.s, runesize);
                    test(r == m.r && runesize == len(m.s));
                    
                    String s;
                    s += m.s;
                    s += '\0';
                    r = utf8::decode(s, runesize);
                    test(r == m.r && runesize == len(m.s));
                    
                    int wantsize = 1;
                    if (wantsize >= len(m.s)) {
                        wantsize = 0;
                    }
                    r = utf8::decode(m.s(0, len(m.s)-1), runesize);
                    test(r == RuneError && runesize == wantsize);

                    s = m.s;
                    if (len(s) == 1) {
                        s[0] = 0x80;
                    } else {
                        s[len(s)-1] = 0x7f;
                    }
                    r = utf8::decode(s, runesize);
                    test(r == RuneError && runesize == 1);
                }
            }
            void test_encode_rune() {
                
            }
            
            void test_decode_rune() {
                
                
            }
        } // utf8
    } // unicode
} // axe


#include "quote.h"

namespace strconv {

//     inline namespace internal {
//         str quote_with(str s, char quote, ascii_only bool, Allocator& alloc) {
//             char rune_tmp[utf8::UTFMax];
//             out.prealloc(3*len(s)/2);
//             buf.append(quote);
//             for (int width = 0; len(s) > 0; s = s(width)) {
//                 rune r = s[0];
//                 width = 1;
//                 if (r >= utf8::RuneSelf) {
//                     r = utf8::decode_rune_in_string(s, width);
//                 }
//                 if (width == 1 && r == utf8::RuneError) {
//                     buf.append({'\\', 'x', lowerhex[s[0]]>>4, lowerhex[s[0]&0xF]});
//                     continue;
//                 }
//                 if (r == quote || r == '\\') { // always backslashed
//                     buf.append({'\\', r});
//                     continue;
//                 }
//             }
//         }
//     }
//     
//     void quote(str s, OutBuf& out) {
//         quote_with(s, '"', false, alloc, out);
//     }
    
}
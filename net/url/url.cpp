#include <utility>
#include <tuple>

#import <axe/utf8/PKG.h>
#import <axe/strings/PKG.h>
#import "PKG.h"

using namespace std;

namespace axe {
namespace url {
    
    namespace  {

        // Maybe s is of the form t c u.
        // If so, return t, c u (or t, u if cutc == true).
        // If not, return s, "".
        void split(str s, char c, bool cutc, str *first, str *rest) {
            size i = strings::index(s, c);
            if (i == npos) {
                *first = s, *rest = "";
                return;
            }
            else if (cutc) {
                *first = s(0,i), *rest = s(i+1);
            } else {
                *first = s(0,i), *rest = s(i);
            }
        }

        // Maybe rawurl is of the form scheme:path.
        // (Scheme must be [a-zA-Z][a-zA-Z0-9+-.]*)
        // If so, return scheme, path; else return "", rawurl.
        str getscheme(str rawurl, str& scheme, str& path) {
            size i = 0;
            for (char c : rawurl) {
                if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z') {
                    // do nothing
                } else if ('0' <= c && c <= '9' || c == '+' || c == '-' || c == '.') {
                    if (i == 0) {
                        scheme = nullptr;
                        path   = rawurl;
                        return nullptr;
                    }
                } else if (c == ':') {
                    if (i == 0) {
                        scheme = nullptr;
                        path   = nullptr;
                        return "Missing protocol scheme";
                    } else {
                        scheme = rawurl(0,i);
                        path   = rawurl(i+1);
                        return nullptr;
                    }
                } else {
                    // we have encountered an invalid character,
                    // so there is no valid scheme
                    scheme = nullptr;
                    path   = rawurl;
                    return nullptr;
                }
                i++;
            }
            scheme = nullptr;
            path   = rawurl;
            return nullptr;
        }

        str fmt_escape_err(str s, Allocator& alloc) {
            //return smt::sprintf(alloc, "invalid URL escape %q", s);
            return "TODO";
        }

        // unescape unescapes a string; the mode specifies
        // which section of the URL string is being unescaped.
        // void 
        // unescape(str s, enctype encoding, Allocator& alloc) {
        //     size n = 0;
        //     bool has_plus = false;
        //     for (int i = 0; i < len(s)) {
        //         switch (s[i]) {
        //         case 'q':
        //             n++;
        //             if (n+2 >= len(s) || !ishex(s[i+1]) || !ishex(s[i+2])) {
        //                 s = s(i);
        //                 if (len(s) > 3) {
        //                     s = s(0,3);
        //                 }
        //                // return {nullptr, fmt_escape_err(s, alloc)};
        //             }
        //             break;
        //         }
        //     }
        // }

        // parse parses a URL from a string in one of two contexts.  If
        // viaRequest is true, the URL is assumed to have arrived via an HTTP request,
        // in which case only absolute URLs or path-absolute relative URLs are allowed.
        // If viaRequest is false, all forms of relative URLs are allowed.
        URL parse(str rawurl, bool via_request, Allocator& alloc) {
            URL url;
            str rest;
            str err;
            
            if (!rawurl && via_request) {
                err = "URL is empty";
                goto Error;
            }
            if (rawurl == "*") {
                url.path = "*";
                return url;
            } 
            
            // Split off possible leading "http:", "mailto:", etc.
            // Cannot contain escaped characters.
            err = getscheme(rawurl, url.scheme, rest);
            if (err) {
                goto Error;
            }
            url.scheme = strings::to_lower_ascii(url.scheme, alloc);
            
            split(rest, '?', true, &rest, &url.raw_query);
            
            if (!strings::has_prefix(rest, '/')) {
                if (url.scheme) {
                    url.opaque = rest;
                    
                }
            }
            
            return url;
            
        Error:
            url.err = ErrParse;
            url.err_extra = err;
            return url;
        }

    } // namespace
        
    // Parse parses rawurl into a URL structure.
    // The rawurl may be relative or absolute.
    URL parse(str rawurl, Allocator& alloc) {
        URL url;
        url.orig = rawurl;
        
        str u, frag;
        split(rawurl, '#', true, &u, &frag);
        url = parse(u, false, alloc);
        if (url.err) {
            return url;
        }
        if (!frag) {
            return url;
        }
        
    //     auto frag_unescaped = unescape(frag, ModeEncodeFragment);
    //     if (frag_unescaped.err) {
    //         url.err = ErrParse;
    //         url.err_extra = frag_unescaped.err;
    //     } else {
    //         ur.fragment = frag_unescaped;
    //     }
    //     
        return url;
    }
}
}
#include <axe/typedefs.h>
#include <axe/str.h>
#include <axe/error.h>

namespace axe {
    namespace url {
        
        const error ErrParse = "Parse error";
        
        struct UserInfo {
            str             username;
            str             password;
            bool            password_set;
        };
        
        struct URL {
            str             scheme;
            str             opaque;
            UserInfo        user;
            str             host;
            str             path;
            str             raw_query;
            str             fragment;
            
            error           err;
            str             err_extra;
            str             orig;
        };
        
        URL parse(str rawurl);
    }
}
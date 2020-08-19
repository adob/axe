#include <cstddef>
#import "typedefs.h"
namespace axe {
    struct Allocator;
    struct error {
        
        using printerfunc = void (*)(const char *str, size len);
        using errorfunc   = void (*)(int code, printerfunc);
        
        // code 00: addr is const char*
        // code 01: code is errno error
        // code 10: addr is function ponter to int (*)(int, printfunc)
        enum ErrType {
            NoErr     = 0,
            StringErr = 0,
            FuncErr   = 1,
            ErrNo     = 2,
            GAIErr    = 3
        } ;
        
        union {
            const char * charp;
            errorfunc   funcp;
            struct {
                int64  code      : 48;
                uint   type      : 2;
                uint   extra     : 14;
            };
        };
        
        
        constexpr error() : charp(0) {}
        constexpr error(int n) : code(n), type(ErrNo), extra(0) {}
        constexpr error(ErrType type, int code=0, uint extra=0) : 
                code(code), type(type), extra(extra) {}
        constexpr error(const char *str) : charp(str) { }
        constexpr error(std::nullptr_t) : code(0), type(0), extra(0) {}
        error(errorfunc f) : funcp(f) {
            type = FuncErr;
        }
        
        error& operator = (std::nullptr_t) { charp = 0; return *this; }
        //error& operator = (error other) { charp = other.charp; return *this; }
        
        constexpr explicit operator bool () const    { return charp != 0; }
        constexpr bool operator ==   (error rhs) { return charp == rhs.charp; }
        constexpr bool operator !=   (error rhs) { return charp != rhs.charp; }
        
        str string(Allocator&) const;

    };
    
    inline bool operator == (error e, std::nullptr_t) { return e.charp == 0; }
    inline bool operator == (std::nullptr_t, error e) { return e.charp == 0; }
    inline bool operator != (error e, std::nullptr_t) { return e.charp != 0; }
    inline bool operator != (std::nullptr_t, error e) { return e.charp != 0; }

}

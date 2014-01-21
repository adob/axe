#include <string>
#import "typedefs.h"
#import "error.h"
#import "backtrace.h"

namespace axe {
#ifndef AXE_EXCEPTION_H
#define AXE_EXCEPTION_H
    struct Exception {
        Backtrace   backtrace;
        std::string        msg;
        
        Exception();
        
        str string(Allocator&) const;
    } ;
    
    namespace exceptions {
        struct BadIndex : Exception {
            BadIndex();
            BadIndex(size got);
            BadIndex(size got, size max);
        };
        
        struct BadMemAccess : Exception {
            BadMemAccess(void *ptr);
        };
        
        struct NullMemAccess : Exception {
            NullMemAccess();
        };
        
        struct AssertionFailed : Exception {
            AssertionFailed(str);
        };
    }

#endif

    namespace exceptions {
        struct Error : Exception {
            Error(error);
        };
    }
}
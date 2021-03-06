#import <axe/fmt/fmt.h>

namespace axe { 

    str Exception::string(Allocator& alloc) const {
        return fmt::sprintf(alloc, "%s\n%s", msg, backtrace);
    } 
    
    namespace exceptions {
        
        BadMemAccess::BadMemAccess(void *ptr) {
            msg = fmt::sprintf("Attempt to dereference invalid pointer at %#.12x", 
                                (uintptr) ptr);
        }


        NullMemAccess::NullMemAccess() {
            msg = "Null dereference";
        }

        Error::Error(error e) {
            msg = fmt::sprintf("Error: %s", e);
        }


        BadIndex::BadIndex() {
            msg = fmt::sprintf("Bad index");
        }
        BadIndex::BadIndex(size got) {
            msg = fmt::sprintf("Bad index: %d", got);
        }
        BadIndex::BadIndex(size got, size max) {
            msg = fmt::sprintf("Index out range [0:%d]: %d", max, got);
        }

        AssertionFailed::AssertionFailed(str s) {
            msg = s;
        }

    }
    
}
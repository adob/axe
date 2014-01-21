#import <axe/exceptions.h>
namespace axe {
    struct errorparam {
        error *err;
        
        errorparam()         : err(nullptr) {}
        errorparam(error& e) : err(&e)      {}
        
        void operator = (error e) {
            try {
                *err = e;
            } catch (exceptions::NullMemAccess) {
                throw exceptions::Error(e);
            }
        }
        
        operator error () {
            if (err) {
                return *err;
            } else {
                return error();
            }
        }
        
        explicit operator bool () {
            if (err) {
                return (bool) *err;
            } else {
                return false;
            }
        }
        
        errorparam& operator = (errorparam const&) = delete;
        str string(Allocator&) const;
    } ;
    
    
    
}
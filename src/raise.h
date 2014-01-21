#import "str.h"
namespace axe {
    void raise(const char*);
    void raise(str msg);
    void raise(int errno_);
    void raise(error e);
}
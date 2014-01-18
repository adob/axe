#include <unordered_set>

#import "hash.h"

namespace axe {
    
    template <typename T>
    struct Set : std::unordered_set<T, Hash> {
        
    } ;
}
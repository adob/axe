#include <unordered_set>

#import <axe/hash/hash.h>

namespace axe {
    
    template <typename T>
    struct Set : std::unordered_set<T, Hash> {
        
    } ;
}
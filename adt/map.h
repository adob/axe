#include <unordered_map>

#import <axe/hash/hash.h>

namespace axe {
    
    template <typename Key, typename Value>
    struct Map : std::unordered_map<Key, Value, axe::Hash> {
        
    } ;
}
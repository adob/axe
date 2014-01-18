#include <unordered_map>

#import "hash.h"

namespace axe {
    
    template <typename Key, typename Value>
    struct Map : std::unordered_map<Key, Value, Hash> {
        
    } ;
}
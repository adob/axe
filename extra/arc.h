#include <string>
#include <memory>

#import <axe/core.h>

namespace axe {
    
struct ARCStr {
    std::shared_ptr<std::string> strp;
    
    ARCStr() : strp(std::make_shared<std::string>()) {}
    
    template <size N>
    ARCStr(const char (&s)[N]) : strp(std::make_shared<std::string>(s, N)) {}
    ARCStr(str s) : strp(std::make_shared<std::string>(s.data, s.len)) {}
    
    operator str () const {
        return str(strp->data(), strp->size());
    }
    
    size write(str s) {
        strp->append(s.data, s.len);
        return s.len;
    }
    
} ;

size len(ARCStr const& s) {
    return s.strp->size();
}

}
#include <ostream>
#include "str.h"

namespace alex {

std::ostream& operator << (std::ostream & o, const str str) {
    o.write(str.data, str.len);
    return o;
}

}
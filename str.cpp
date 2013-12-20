#include <ostream>
#include "str.h"

std::ostream& operator << (std::ostream & o, const strref str) {
    o.write(str.data, str.len);
    return o;
}

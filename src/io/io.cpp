#include "io.h"
namespace axe {
namespace io {



}

esize write(FILE *f, str s) {
    size n = fwrite(s.data, 1, s.len, f);
    if (n != s.len) {
        if (feof(f)) {
             return {n, errno};
        } else {
            return {n, "Uknown error while writing to file"};
        }
    }
    return {n, nil};
}
}
#include <string.h>
#include <unistd.h>

#import "str.h"

namespace axe {

    TempCStr strref::tmp_c_str() {
        return TempCStr(data, len);
    }

    TempCStr::TempCStr(const char *data, size len) {
        if (data == nullptr) {
            charp = nullptr;
            return;
        }

        if (len > 255) {
            charp = (char*) malloc(len+1);
        } else {
            charp = buf;
        }
        memcpy(charp, data, len);
        charp[len] = '\0';
    }

    TempCStr::operator const char * () {
        return charp;
    }
        
    TempCStr::~TempCStr() {
        if (charp != buf) {
            free(charp);
        }
    }

}
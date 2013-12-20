#pragma once
#include "builtin.h"

namespace os {
    
struct File {
    error   err;
    int     fd;
    strbuf  name;
    
    void init(int fd, str name);
    size read(byteref b, error&);
    size read(byteref b);
    
    ~File() {
        // close();
    }
};

File create(str name,  error& err);
File create(str name);
error create(str name, File& file);

File open_file(str name, int flag, mode_t perm, error& err);
File open_file(str name, int flag, mode_t perm);
error open_file(str name, int flag, mode_t perm, File& file);

File open(str name, error& err);
File open(str name);
error open(str name, File& file);

}

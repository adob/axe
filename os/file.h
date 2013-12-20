#pragma once
#include "basic.h"
#include "alex/syscall"

namespace alex {
namespace os {

using error = syscall::error;
    
struct File {
    int     fd;
    strbuf  name;
    
    void init(int fd, str name);
    size read(slice<char> b, error&);
    size read(slice<char> b);
    
    ~File() {
        // close();
    }
};



//using filemode = mode_t;

// enum errortype {
//     OK,
//     PathError,
//     LinkError
// };

// struct error {
//     type        errortype;
//     err         syscall::error;
// };

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
}
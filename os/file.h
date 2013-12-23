#pragma once
#include "builtin.h"
#include <fcntl.h>


namespace os {
    const error ErrInvalid = "Invalid file";
    const error eof        = "EOF";

    struct File {
        error   err;
        int     fd;
        Str     name;
        
        void init(int fd, str name=nil);
        
        size read(bufref b, error&);
        size pread_fully(bufref b, size offset, error&);
        
        File() : err(ErrInvalid), fd(-1) {}
        //File(str name, error& err);
        //File(str name);
        ~File() {
            // close();
        }
    };



    File create(str name, int flag = O_RDWR|O_CREAT|O_TRUNC, mode_t = 0666);
    File create(const char *name, int flag = O_RDWR|O_CREAT|O_TRUNC, mode_t = 0666);

    File open(str name, int flag = O_RDONLY, mode_t = 0);
    File open(const char *name, int flag = O_RDONLY, mode_t = 0);
}

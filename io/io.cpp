#include <unistd.h>
#import "io.h"


namespace axe { namespace io {

size write(FILE *f, str s, errorparam err) {
    size n = fwrite(s.data, 1, s.len, f);
    if (n != len(s)) {
        if (feof(f)) {
            return err=EOF, n;
        } else {
            return err=errno, n;
        }
    }
    return n;
}

size write(int fd, str s, errorparam err) {
    size ret = ::write(fd, s.data, s.len);
    if (ret == -1) {
        return err=errno, 0;
    } else if (ret < len(s)) {
        return err=EOF, ret;
    }
    return ret;
}

size read(FILE *file, buf b, errorparam err) {
    //printf("fread");
    size n = fread(b.data, 1, len(b), file);
    if (n != len(b)) {
        if (feof(file)) {
            return n;
        } else {
            return err=errno, n;
        }
    }
    return n;
}

size read(int fd, buf b, errorparam err) {
    size ret = ::read(fd, b.data, b.len);
    if (ret == -1) {
        return err=errno, 0;
    }
    return ret;
}
    


}}
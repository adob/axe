#include <unistd.h>
#include <axe/io.h>


namespace axe { namespace io {


str write(FILE *f, str s, errorparam err) {
    size n = fwrite(s.data, 1, s.len, f);
    if (n != len(s)) {
        if (feof(f)) {
            return s(0, n);
        } else {
            return err=errno, s(0, n);
        }
    }
    return s(0, n);
}

str write(int fd, str s, errorparam err) {
    size ret = ::write(fd, s.data, s.len);
    if (ret == -1) {
        return err=errno, str();
    }
    return s(0, ret);
}

buf read(FILE *file, buf b, errorparam err) {
    //printf("fread");
    size n = fread(b.data, 1, len(b), file);
    if (n != len(b)) {
        if (feof(file)) {
            return b(0, n);
        } else {
            return err=errno, b(0, n);
        }
    }
    return b(0, n);
}

buf read(int fd, buf b, errorparam err) {
    size ret = ::read(fd, b.data, b.len);
    if (ret == -1) {
        return err=errno, buf();
    }
    return b(0, ret);
}

}}
#import "bufio/bufio.h"
#import "bufio/reader.h"
#import "bufio/writer.h"

namespace axe {
    template <typename T> using BufReader = bufio::Reader<T>;
    template <typename T> using BufWriter = bufio::Writer<T>;
}
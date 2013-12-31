constexpr bufref::bufref() : data(0), len(0) {}
constexpr bufref::bufref(char *data, size len) : data((byte*)data), len(len) {}
constexpr bufref::bufref(byte *data, size len) : data(data), len(len) {}
constexpr bufref::operator strref const () {
    return strref((char*)data, len);
}

constexpr char *begin(bufref b)             { return (char*) b.data; }
constexpr char *end(bufref b)               { return (char*) (b.data+b.len); }
constexpr size  len(bufref b)               { return b.len; }

constexpr strref::strref() : data(nullptr), len(0) {}
//constexpr strref::strref(const bufref b) : data(b.data), len(b.len) {}
constexpr strref::strref(nullptr_t) : data(nullptr), len(0) {}
constexpr strref::strref(const char *str, size strlen) : data(str), len(strlen) {}
template <size N> constexpr strref::strref(const char (&str)[N]) : data(str), len(N-1){}
template <size N> constexpr strref::strref(const uint8 (&bytes)[N]) : data((char*)bytes), len(N) { }

constexpr strref strref::slice(size i) const {
    return assert(i <= len), 
    strref(data+i, len-i);
}

constexpr strref strref::slice(size i, size j) const {
    return assert(i <= len && i <= j),
    strref(data+i, j-i);
}

constexpr char strref::operator [] (size i) const {
    return assert(i < len),
    data[i];
}

constexpr strref strref::operator()(size i, size j) const {
    return slice(i, j);
}

constexpr strref strref::operator()(size i) const  {
    return slice(i);
}

constexpr const char *begin(strref s)       { return s.data; }
constexpr const char *end(strref s)         { return s.data+s.len; }
constexpr size        len(strref s)         { return s.len; }

constexpr bool operator == (strref left, strref right) {
    return (left.len != right.len) ?
        false :
            left.data == right.data ?
                true : memcmp(left.data, right.data, left.len) == 0;
}

constexpr bool operator != (strref left, strref right) {
    return !(left == right);
}


constexpr bool operator == (strref left, nullptr_t) {
    return left.data == nullptr;
}
constexpr bool operator == (nullptr_t, strref right) {
    return right.data == nullptr;
}

// Buf
template <typename ArgFirst, typename ArgSecond, typename... ArgRest>
void Buf::append(const ArgFirst&  a1, const ArgSecond& a2, const ArgRest&... args) {
    append(a1);
    append(a2, args...);
}

template <typename T>
Buf& Buf::operator += (T const& s) {
    append(s);
    return *this;
}


// SmallBuf
template <size N>
SmallBuf<N>::SmallBuf(Allocator& alloc) : Buf(alloc, fixedbuf, N) {
    
}


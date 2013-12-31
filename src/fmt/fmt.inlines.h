namespace internal {
    
    struct Opts {
        size width     = 0;
        uint prec      = 0;
        uint base      = 10;
        
        union {
            uint32 flags = 0;
            struct {
                bool upcase     : 1;
                bool plus       : 1;
                bool minus      : 1;
                bool zero       : 1;
                bool sharp      : 1;
                bool space      : 1;
                bool comma      : 1;
                bool apos       : 1;
                bool prec_present : 1;
            } ;
        } ;
        
        Opts() : width(0), prec(0), base(10), flags(0) {}
    } ;
    
    struct PrettyPrinter : Opts {
        Str& out;
        
        PrettyPrinter(Str& out) : out(out) {}
        PrettyPrinter(Str& out, Opts const& opts) : Opts(opts), out(out) {}
        
        void pad(str s) {
            pad(s, len(s));
        }
        
        void pad(str s, size runecount){
            if (width == 0 || runecount >= width) {
                out += s;
                return;
            }
            bool left = !minus;
            size padamt = width - runecount;
            char padchar = zero ? '0' : ' ';
            
            if (left) {
                out.append_many(padchar, padamt);
                out += s; 
            } else {
                out += s;
                out.append_many(padchar, padamt);
            }
        }
        
        template <typename T>
        void write_integer(T n) {
            char buf[256];
            static const str ldigits = "0123456789abcdefghijklmnopqrstuvwxyz";
            static const str udigits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            const str digits = upcase ? udigits : ldigits;
            char sep = comma ? ',' : apos ? '\'' : 0;
            int nsep = (base == 16 || base == 2) ? 5 : 4;
            
            bool negative = n < 0;
            if (negative)
                n = -n;
            
            size i = sizeof buf;
            size ncount = 0;
            while (n > 0) {
                ncount++;
                if (sep && (ncount % nsep) == 0) {
                    buf[--i] = sep;
                    ncount++;
                }
                buf[--i] = digits[n % base];
                n /= base;
            }
            
            size pad_prec = prec > ncount ? prec - ncount : 0;
            size sharp_extra = !sharp ? 0
                                    : (base == 16 || base == 2) ? 2
                                        : base == 8 ? 1
                                            : 0;
            ncount += pad_prec + sharp_extra;
            if (negative||plus||space) ncount++;
            size pad_width = width > ncount ? width-ncount : 0;

            if (pad_width > 0) {
                char padchar = zero ? '0' : ' ';
                out.append_many(padchar, pad_width);
            }
            if (negative)
                out.append('-');
            else if (plus)
                out.append('+');
            else if (space)
                out.append(' ');
            if (sharp) {
                if (base == 8)       out.append('0');
                else if (base == 2)  out.append("0b");
                else if (base == 16) out.append("0x");
            }
            if (pad_prec > 0) {
                out.append_many('0', pad_prec);
            }
            out.append(str(buf+i, sizeof(buf)-i));
        }
        
        void write(int n) {
            write_integer(n);
        }
        
        void write(unsigned n) {
            write_integer(n);
        }
        
        void write(long n) {
            write_integer(n);
        }
        
        void write (long unsigned n) {
            write_integer(n);
        }
        
        void write (__int128 n) {
            write_integer(n);
        }
        
        void write(unsigned __int128 n) {
            write_integer(n);
        }
        
        void write(double n) {
            char buf[256];
            int count = snprintf(buf, sizeof buf, "%g", n);
            pad(str(buf, count), count);
        }
        
        void write(float f) {
            write((double) f);
        }
        
        void write(bool b) {             
            pad(b ? "true" : "false");
        }
        
        void write(str s) {
            size runecount = utf8::count(s);
            pad(s, runecount);
        }
        
        void write(const char *charp) {
            write(str(charp));
        }
        
    } ;
    
    
    
    template <size N, typename... Args> struct PrinterT;
    
    template <size N, typename Arg, typename... Args>
    struct PrinterT<N, Arg, Args...> : PrinterT<N+1, Args...> {
        Arg const& arg;
        
        PrinterT(Arg const& a, const Args&... args) : PrinterT<N+1, Args...>(args...), arg(a) {}
        
        bool doprint(size n, PrettyPrinter& out) {
            if (n == N) {
                out.write(arg);
                return true;
            }
            return static_cast<PrinterT<N+1, Args...>>(*this).doprint(n, out);
        }
        
        size getsize() {
            return static_cast<PrinterT<N+1, Args...>>(*this).getsize();
        }
        
        size touint(int n) {
            return n;
        }
        
        template <typename T>
        size touint(T const&, PrettyPrinter& out) {
            char buf[256];
            ::snprintf(buf, sizeof buf, "<![can't convert arg %d to int]>", (int)N+1);
            out.write(buf);
            return 0;
        }
        
        size getuint(size n, PrettyPrinter& out) {
            if (n == N) {
                return touint(arg, out);
            }
            return static_cast<PrinterT<N+1, Args...>>(*this).getuint(n, out);
        }
    } ;
    
    template <size N>
    struct PrinterT<N> {
        
        bool doprint(size n, PrettyPrinter& out) {
            char buf[256];
            ::snprintf(buf, sizeof buf, "%%(MISSING ARG %d)", (int)n+1);
            out.write(buf);
            return false;
        }
        
        size getuint(size, PrettyPrinter& out) {
            out.write("<missing int arg>");
            return 0;
        }
        
        size getsize() { return N; }
    } ;
            
    template <typename... Args>
    str sprintf(Allocator& alloc, str format, const Args&... args) {
        PrinterT<0, Args...> printer(args...);
        Str outstr(alloc);
        size argi  = 0;
        size lasti = 0;
        size i     = (size) -1;
        int state  = 0;
        bool percent = false;
        PrettyPrinter out(outstr);
        
        for (char c : format) {
            i++;
            if (c == '%') {
                outstr += format(lasti, i);
                lasti = i;
                percent = !percent;
                state = 0;
                out.flags     = 0;
                out.width     = 0;
                out.prec      = 0;
                out.base      = 10;
            } else if (percent) {
                switch (c) {
                    case 'X': 
                        out.upcase = true;
                    case 'x':
                        out.base = 16;
                        break;
                    case 'O':
                        out.upcase = true;
                    case 'o':
                        out.base = 8;
                        break;
                    case 'b':
                        out.base = 2;
                        break;
                        
                    case ',':
                        out.comma = true;
                        continue;
                    case '\'':
                        out.apos = true;
                        continue;
                    case '+':
                        out.plus = true;
                        continue;
                    case '-':
                        out.minus = true;
                        continue;
                    case '#':
                        out.sharp = true;
                        continue;
                    case ' ':
                        out.space = true;
                        continue;
                    case '.': 
                        state = 2; 
                        continue;
                    case '*':
                        if (state < 2) {
                            out.width = printer.getuint(argi++, out);
                        } else {
                            out.prec = printer.getuint(argi++, out);
                        }
                        continue;
                    case '0': 
                        if (state == 0) {
                            out.zero = true; 
                            continue;
                        }
                    default: 
                        if ('0' <= c && c <= '9') {
                            if (state == 0)
                                state = 1;
                            if (state == 1)
                                out.width = out.width*10 + c-'0';
                            else {
                                out.prec_present = true;
                                out.prec = out.prec*10 + c-'0';
                            }
                                
                        } else {
                            break;
                        }
                        continue;
                }
                printer.doprint(argi++, out);
                lasti = i + 1;
                percent = false;
            }
        }
        if (++i != lasti) {
            outstr += format(lasti, i);
        }
        return outstr;
    }
}

template <typename... Args>
str sprintf(Allocator& alloc, str format, Args... args) {
    return internal::sprintf(alloc, format, args...);
}

template <typename... Args>
void printf(str format, Args... args) {
    Allocator alloc;
    str s = internal::sprintf(alloc, format, args...);
    write(stdout, s);
}

template <typename... Args>
std::string sprintf(str format, Args... args) {
    Allocator alloc;
    str s = internal::sprintf(alloc, format, args...);
    return s;
}

//     template <typename T>
//     std::string stringify(T const& t) {
//         Allocator alloc;
//         Str s(alloc);
//         internal::PrettyPrinter printer(s);
//         printer.write(t);
//         return s;
//     }

template <typename T>
std::string stringify(T const& t) {
    Allocator alloc;
    return stringify(t, alloc);
}

template <typename T>
str stringify(T const& t, Allocator& alloc) {
    Str s(alloc);
    internal::PrettyPrinter printer(s);
    printer.write(t);
    
    return s;
}

//static_assert()



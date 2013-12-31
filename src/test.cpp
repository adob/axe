// #include <csignal>
// #include <execinfo.h>
// #include <unistd.h>
//#include <ostream>
// #include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include <tuple>
#include <utility>
#include <array>
#include <vector>
#include <chrono>

#include <axe/core.h>
#include <axe/unicode/utf8.h>
#include <axe/strconv.h>
#include <axe/fmt.h>
#include <axe/net.h>
#include <axe/time.h>
#include <axe/print.h>




// struct Foo {
//     static const int a = 7;
//     static const int b = 8;
// };
// 
// template <typename T>
// struct floatinfo { };
// 
// template <>
// struct floatinfo<float> {
//     static const uint mantbits = 23;
//     static const uint expbits  = 8;
//     static const  int bias     = -127;
//     
//     typedef uint32 manttype;
// };
// 
// template <>
// struct floatinfo<double> {
//     static const uint mantbits = 52;
//     static const uint expbits  = 11;
//     static const  int bias     = -1023;
//     
//     typedef uint64 manttype;
// };
// 
// str ffloat(Allocator& alloc, double f) {
//     uint64 bits = f;
//     const uint mantbits = floatinfo<double>::mantbits;
//     const uint expbits  = floatinfo<double>::expbits;
//     const int  bias     = floatinfo<double>::bias;
//     using manttype      = floatinfo<double>::manttype;
//     
//     
//     bool     neg  = bits >> (expbits + mantbits);
//     int      exp  = bits >> mantbits & ((1 << expbits) - 1);
//     manttype mant = bits & ((((manttype)1) << mantbits) - 1);
//     
//     switch (exp) {
//         case (1 << expbits) - 1:
//             if (mant != 0) return "NaN";
//             if (neg)       return "-Inf";
//                            return "+Inf";
//         case 0:
//             // denormalized
//             exp++;
//         default:
//             // add implicit bias
//             mant |= ((manttype)1) << mantbits;
//     }
//     exp += bias;
//     
//     print fmt::sprintf(alloc, "%b", 8);
//     return "";
// }



// void foo(int i, errorparam e = {}) {
//     
// }
// 
// struct S {
//     size width;
//     union {
//         uint32 flags = 0;
//         struct {
//             bool upcase     : 1;
//             bool plus       : 1;
//             bool minus      : 1;
//             bool zero       : 1;
//             bool sharp      : 1;
//             bool space      : 1;
//             bool comma      : 1;
//             bool apos       : 1;
//             bool prec_present : 1;
//         } ;
//     } ;
// } ;
// 
// struct S2 {
//    char data[128];
// } ;
// 
// void printdiff(timespec const& t1, timespec const& t2) {
//     int64 time = (t2.tv_sec - t1.tv_sec) * 1000000000;
//     time += t2.tv_nsec - t1.tv_nsec;
//     
//     print time;
// }
// 
// using int128 = __int128;
// 
// struct Time {
//     int128 nsecs;
// } ;
// 
// Time monotime() {
//     timespec ts;
//     clock_gettime(CLOCK_MONOTONIC, &ts);
//     return Time{ts.tv_sec * 1000000000  + ts.tv_nsec};
//     
//     Time::Now()
//     
//     Fmt::Sprintf()
//     
// }
// 
// int128 operator - (Time t1, Time t2) {
//     return t1.nsecs - t2.nsecs;
// }

struct S1 {
    char buf[100];
};


using namespace axe;
int main() {
    
    std::vector<S1> vec;
    int count = 1000000;
    
    int128 start = time::monotonic();
    for (int i = 0; i < count; i++) {
        S1 s1;
        s1.buf[0] = 'a';
        vec.push_back(s1);
    }
    int128 end   = time::monotonic();
   
    fmt::printf("%,d ns\n", end-start);
    
    Allocator alloc(512);
    List<S1> list(alloc);
    start = time::monotonic();
    for (int i = 0; i < count; i++) {
        S1& s1 = *list.make();
        s1.buf[0] = 'a';
    }
    end   = time::monotonic();
    
    fmt::printf("%,d ns\n", end-start);
    return 0;
}
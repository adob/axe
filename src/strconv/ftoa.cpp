#include "core.h"


namespace axe {

str format_float(Allocator& alloc, double f, str mft, int prec, int bitsize) {
    uint64 bits = f;
    //bool neg = f
}

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



}
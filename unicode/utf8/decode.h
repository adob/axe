#import <axe/typedefs.h>
#import <axe/str.h>

namespace axe {
    namespace unicode {
        namespace utf8 {
            /// decode(s[, nbytes]) unpacks the first UTF-8 encoding in s and returns the rune with
            /// its width in bytes stored in nbytes, if present. If the encoding is invalid, RuneError
            /// is returned and nbytes is 1, an impossible result for correct UTF-8.
            /// An encoding is invlalid if it is incorrect UTF-8, encodes a rune that is out
            /// range, or is not the shortest possible UTF-8 encoding for the value.
            /// No other validation is performed.
            rune decode(str s, int& nbytes);
            rune decode(str s);
            
            /// decode_last(s[, nbytes]) unpacks the last UTF-8 encoding in s and return the rune
            /// with its width in bytes stored in nbytes, if present.
            /// If the encoding is invalid, RuneError
            /// is returned and nbytes is 1, an impossible result for correct UTF-8.
            /// An encoding is invlalid if it is incorrect UTF-8, encodes a rune that is out
            /// range, or is not the shortest possible UTF-8 encoding for the value.
            /// No other validation is performed.
            rune decode_last(str s, int& nbytes);
            rune decode_last(str s);
            
            
            /// count(s) returns the number of runes in s. Erroneous and short encodings
            /// are treated as runes of width 1 byte.
            size count(str s);
            
            /// full_rune(s) reports whether the bytes in s begin with a full UTF-8 encoding of a  rune.
            /// An invalid encoding is considered a full rune since it will convert as a width-1 error rune.
            bool full_rune(str s);
            
            struct runestr {
                str s;
                
                struct RuneIterator {
                    str          s;
                    rune         r;
                    
                    void operator ++ ();
                    rune operator * ();
                    bool operator != (RuneIterator const&);
                } ;
                
                RuneIterator begin();
                RuneIterator end();
            } ; 

            runestr runes(str s);
        }
    }
}
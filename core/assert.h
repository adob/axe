#include <stdexcept>

#ifdef NDEBUG
#ifdef assert
#undef assert
#endif
#define assert(arg) (static_cast<void> (0))
#else
#define assert(arg) ((arg) ? (static_cast<void> (0)) : (throw std::logic_error("assertion failed")))
#endif

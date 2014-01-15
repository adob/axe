#include <stdexcept>

#ifdef NDEBUG
#ifdef assert
#undef assert
#endif
#define assert(arg) (static_cast<void> (0))
#else
#define assert(check, T, ...) ((check) ? (static_cast<void> (0)) : (throw T(__VA_ARGS__)))
#endif

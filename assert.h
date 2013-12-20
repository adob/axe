#pragma once
#include <stdexcept>

#ifdef NDEBUG
#define assert(arg) (static_cast<void> (0))
#else
#define assert(arg) ((arg) ? (static_cast<void> (0)) : (throw std::logic_error("assertion failed")))
#endif
#ifndef ASSERTIONS_HPP
#define ASSERTIONS_HPP

#include <cassert>

#ifdef DEBUG
# define ASSERT(condition, message) assert(condition && message)
#else
# define ASSERT(condition, message)
#endif

#endif  // ASSERTIONS_HPP

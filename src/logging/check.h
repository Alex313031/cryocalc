#ifndef MINI_LOGGER_CHECK_H_
#define MINI_LOGGER_CHECK_H_

#include <intrin.h>

#include "logging_base.h"

// DCHECK is a debug-only assertion macro. It checks if the condition is true,
// and if not, triggers a debug break. In release builds (NDEBUG defined),
// the condition is not evaluated.
//
// Usage:
//   DCHECK(ptr != nullptr);
//   DCHECK(index < array_size);
//   DCHECK(number >= int);
// clang-format off
// Macro to convert to string
#if !defined(_STRINGIZER_)
 #define _STRINGIZER_
 #define _STRINGIZER(in) #in
 #define STRINGIZE(in) _STRINGIZER(in)
#endif // !defined(_STRINGIZER_)
// clang-format on

namespace logging {

  // Function that runs LOG(FATAL)
  void CheckImpl(const char* condition, bool check_flag);

  // for NOTREACHED()
  void NotReachedImpl();

} // namespace logging

#define CHECK(condition) logging::CheckImpl(STRINGIZE(condition), !(condition))

#if !defined(NDEBUG)
 #define DCHECK(condition) CHECK(condition)
#else // NDEBUG defined (release build)
 #define DCHECK(condition) ((void)0)
#endif // !defined(NDEBUG)

#define NOTREACHED() logging::NotReachedImpl()

#endif // MINI_LOGGER_CHECK_H_

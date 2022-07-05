#pragma once

#include <cstdio>

// FIXME: always on debug mode
#define GEAR_DEBUG_MODE

#define GEAR_ASSERT_IMPL(type, cond, sep, msg) do { if (!(cond)) { fprintf(stderr, "%s:%d: " type " Failed '" #cond "'%s%s\n", __FILE__, __LINE__, sep, msg); } std::exit(1); } while(false)

#ifdef GEAR_DEBUG_MODE
# define GEAR_DEBUG_ASSERT(cond)                   GEAR_ASSERT_IMPL("Debug Assert", cond, "", "")
# define GEAR_DEBUG_ASSERT_WITH_MESSAGE(cond, msg) GEAR_ASSERT_IMPL("Debug Assert", cond, ": ", msg)
#else
# define GEAR_DEBUG_ASSERT(cond, msg) (void)(cond)
# define GEAR_DEBUG_ASSERT_WITH_MESSAGE(cond, msg) (void)(cond)
#endif

#define GEAR_ASSERT(cond)                         GEAR_ASSERT_IMPL("Assert", cond, "", "")
#define GEAR_ASSERT_WITH_MESSAGE(cond, msg)       GEAR_ASSERT_IMPL("Assert", cond, ": ", msg)

#pragma once

#include <cstdio>

// FIXME: always on debug mode
#define GAME_DEBUG_MODE

#define GAME_ASSERT_IMPL(type, cond, sep, msg) do { if (!(cond)) { fprintf(stderr, "%s:%d: " type " Failed '" #cond "'%s%s\n", __FILE__, __LINE__, sep, msg); std::exit(1); } } while(false)

#ifdef GAME_DEBUG_MODE
# define GAME_DEBUG_ASSERT(cond)                   GAME_ASSERT_IMPL("Debug Assert", cond, "", "")
# define GAME_DEBUG_ASSERT_WITH_MESSAGE(cond, msg) GAME_ASSERT_IMPL("Debug Assert", cond, ": ", msg)
#else
# define GAME_DEBUG_ASSERT(cond, msg) (void)(cond)
# define GAME_DEBUG_ASSERT_WITH_MESSAGE(cond, msg) (void)(cond)
#endif

#define GAME_ASSERT(cond)                         GAME_ASSERT_IMPL("Assert", cond, "", "")
#define GAME_ASSERT_WITH_MESSAGE(cond, msg)       GAME_ASSERT_IMPL("Assert", cond, ": ", msg)

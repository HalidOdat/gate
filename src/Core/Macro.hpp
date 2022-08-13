#pragma once

#include <cstdio>
#include <cstdlib>

// FIXME: always on debug mode
#define GAME_DEBUG_MODE _DEBUG

#define GAME_ASSERT_IMPL(type, cond, sep, msg) do { if (!(cond)) { fprintf(stderr, "%s:%d: " type " Failed '%s'%s%s\n", __FILE__, __LINE__, #cond, sep, msg); std::exit(1); } } while(false)

#if GAME_DEBUG_MODE
# define GAME_DEBUG_ASSERT(cond)                   GAME_ASSERT_IMPL("Debug Assert", cond, "", "")
# define GAME_DEBUG_ASSERT_WITH_MESSAGE(cond, msg) GAME_ASSERT_IMPL("Debug Assert", cond, ": ", msg)
#else
# define GAME_DEBUG_ASSERT(cond, msg) (void)(cond)
# define GAME_DEBUG_ASSERT_WITH_MESSAGE(cond, msg) (void)(cond)
#endif

#define GAME_ASSERT(cond)                         GAME_ASSERT_IMPL("Assert", cond, "", "")
#define GAME_ASSERT_WITH_MESSAGE(cond, msg)       GAME_ASSERT_IMPL("Assert", cond, ": ", msg)

#define GAME_UNREACHABLE(msg) do { fprintf(stderr, "Unreachable %s:%d: %s\n", __FILE__, __LINE__, (msg)); exit(1); } while(false)
#define GAME_TODO(msg)        do { fprintf(stderr, "Todo %s:%d: %s\n", __FILE__, __LINE__, (msg)); exit(1); } while(false)

namespace Game {
  void CheckOpenGLError(const char* stmt, const char* fname, int line);
} // namespace Game

#if GAME_DEBUG_MODE
# define GAME_GL_CHECK(stmt) do { \
    stmt; \
    Game::CheckOpenGLError(#stmt, __FILE__, __LINE__); \
  } while (0)
#else
# define GAME_GL_CHECK(stmt) stmt
#endif

#ifdef _MSC_VER
# define _CRT_SECURE_NO_WARNINGS 1
#endif
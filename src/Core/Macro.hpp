#pragma once

#include <cstdio>
#include <cstdlib>

#if defined(__EMSCRIPTEN__)
  #define GATE_PLATFORM_WEB 1
#else
  #define GATE_PLATFORM_NATIVE 1
#endif

#if !defined(GATE_DEBUG_MODE) && !defined(GATE_RELEASE_MODE)
# define GATE_DEBUG_MODE 1
#endif

#define GATE_ASSERT_IMPL(type, cond, sep, msg) do { if (!(cond)) { fprintf(stderr, "%s:%d: " type " Failed '%s'%s%s\n", __FILE__, __LINE__, #cond, sep, msg); fflush(stdout); fflush(stderr); std::exit(1); } } while(false)

#ifdef GATE_DEBUG_MODE
# define GATE_DEBUG_ASSERT(cond)                   GATE_ASSERT_IMPL("Debug Assert", cond, "", "")
# define GATE_DEBUG_ASSERT_WITH_MESSAGE(cond, msg) GATE_ASSERT_IMPL("Debug Assert", cond, ": ", msg)
#else
# define GATE_DEBUG_ASSERT(cond) (void)(cond)
# define GATE_DEBUG_ASSERT_WITH_MESSAGE(cond, msg) (void)(cond)
#endif

#define GATE_ASSERT(cond)                         GATE_ASSERT_IMPL("Assert", cond, "", "")
#define GATE_ASSERT_WITH_MESSAGE(cond, msg)       GATE_ASSERT_IMPL("Assert", cond, ": ", msg)

#define GATE_UNREACHABLE(msg) do { fprintf(stderr, "Unreachable %s:%d: %s\n", __FILE__, __LINE__, (msg)); exit(1); } while(false)
#define GATE_TODO(msg)        do { fprintf(stderr, "Todo %s:%d: %s\n", __FILE__, __LINE__, (msg)); exit(1); } while(false)

#ifdef _MSC_VER
# define _CRT_SECURE_NO_WARNINGS 1
#endif
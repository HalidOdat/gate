#pragma once

#include "Core/Type.hpp"

#include <chrono>
#include <cstdio>

namespace Game::Debug {
  
  using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

  class Profiler {
  public:
    struct Session {
      String name;
      String file;
      FILE*  fd;
    };

    struct TimerResult {
      const char* name;
      
      FloatingPointMicroseconds start;
      std::chrono::microseconds elapsedTime;
      u32 threadId = 0;
    };

  public:
    DISALLOW_MOVE_AND_COPY(Profiler);
    ~Profiler();

    static Profiler& get();

    void beginSession(String name);
    void endSession();

    void report(const TimerResult& result);

  private:
    Profiler() = default;

  private:
    static Profiler sInstance;

  private:
    Session* mCurrentSession = nullptr;
  };

  class Timer {
  public:
    inline Timer(const char* name)
      : mName{name}, mStartPoint{std::chrono::steady_clock::now()}
    {}
    ~Timer();

  private:
    const char* mName;
    std::chrono::time_point<std::chrono::steady_clock> mStartPoint;
  };

#if GAME_PROFILE

# if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#   define GAME_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
# elif defined(__DMC__) && (__DMC__ >= 0x810)
#   define GAME_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
# elif (defined(__FUNCSIG__) || (_MSC_VER))
#   define GAME_FUNCTION_SIGNATURE __FUNCSIG__
# elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#   define GAME_FUNCTION_SIGNATURE __FUNCTION__
# elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#   define GAME_FUNCTION_SIGNATURE __FUNC__
# elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#   define GAME_FUNCTION_SIGNATURE __func__
# elif defined(__cplusplus) && (__cplusplus >= 201103)
#   define GAME_FUNCTION_SIGNATURE __func__
# else
#   error "no known function signature macro found!"
# endif

# define GAME_PROFILE_BEGIN_SESSION(name) ::Game::Debug::Profiler::get().beginSession(name)
# define GAME_PROFILE_END_SESSION()       ::Game::Debug::Profiler::get().endSession()
# define GAME_PROFILE_FUNCTION()          auto _timer__##__FILE__##__LINE__ = ::Game::Debug::Timer(GAME_FUNCTION_SIGNATURE)

#else

# define GAME_PROFILE_BEGIN_SESSION(name)
# define GAME_PROFILE_END_SESSION()
# define GAME_PROFILE_FUNCTION()

#endif

} // namespace Game::Debug

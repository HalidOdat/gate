#include "Core/Base.hpp"
#include "Debug/Profiler.hpp"

#include <filesystem>

namespace Game::Debug {
  
  static constexpr const auto PRFILES_DIRECTORY = "profiles/";

  Profiler Profiler::sInstance;

  Profiler::~Profiler() {
    if (mCurrentSession) {
      delete mCurrentSession;
      mCurrentSession = nullptr;
    }
  }

  Profiler& Profiler::get() {
    return sInstance;
  }
  void Profiler::beginSession(String name){
    if (mCurrentSession != nullptr) {
      Logger::error("Profiler: Session %s has not ended", mCurrentSession->name.c_str());
      endSession();
    }

    Logger::info("Profiler: Starting session %s", name.c_str());

    if (!std::filesystem::is_directory(PRFILES_DIRECTORY)) {
      if (!std::filesystem::create_directory(PRFILES_DIRECTORY)) {
        Logger::error("Profiler: Couldn't create %s directory", PRFILES_DIRECTORY);  
        exit(1);
      }
    }

    auto path = PRFILES_DIRECTORY + name + ".json";
    FILE* fd = fopen(path.c_str(), "w+");
    if (!fd) {
      Logger::error("Profiler: Couldn't open file '%s' for writing", path.c_str());
      exit(1);
    }

    mCurrentSession = new Profiler::Session{std::move(name), std::move(path), fd};

    // Writing header
    fprintf(mCurrentSession->fd, "{\"otherData\": {},\"traceEvents\":[{}");
  }

  void Profiler::endSession() {
    Logger::info("Profiler: Ending session %s", mCurrentSession->name.c_str());
    
    // Writing footer
    fprintf(mCurrentSession->fd, "]}");
    fflush(mCurrentSession->fd);
    fclose(mCurrentSession->fd);

    delete mCurrentSession;
    mCurrentSession = nullptr;
  }

  void Profiler::report(const TimerResult& result) {
    if (!mCurrentSession) {
      Logger::error("Profiler: timer '%s' report before session creation", result.name);
      exit(1);
    }
    fprintf(mCurrentSession->fd,
      ",{"
      "\"cat\":\"function\","
      "\"dur\":%llu,"
      "\"name\":\"%s\","
      "\"ph\":\"X\","
      "\"pid\":0,"
      "\"tid\":%u,"
      "\"ts\":%.3f"
      "}",
      result.elapsedTime.count(),
      result.name,
      result.threadId,
      result.start.count()
    );
  }

  Timer::~Timer() {
    auto finishTime   = std::chrono::steady_clock::now();
    auto endTimepoint = std::chrono::steady_clock::now();
		auto highResStart = FloatingPointMicroseconds{ mStartPoint.time_since_epoch() };
		auto elapsedTime  = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(mStartPoint).time_since_epoch();

		Profiler::get().report({ mName, highResStart, elapsedTime, 0 });
  }

} // namespace Game::Debug

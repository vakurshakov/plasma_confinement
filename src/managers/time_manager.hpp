#ifndef SRC_MANAGERS_TIME_MANAGER_HPP
#define SRC_MANAGERS_TIME_MANAGER_HPP

// Basic instrumentation profiler by Cherno.

#include <omp.h>

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

struct Profile_result {
  std::string name;  // recorded profile;
  int64_t start;     // profile beginning in microseconds;
  int64_t duration;  // elapsed time in microseconds;
  int thread_num;    // thread number given by omp call.
};

class Instrumentor {
 public:
  Instrumentor()
  : profile_count_(0) {}

  void begin_session(const std::string& filepath = "results.json") {
    output_stream_.open(filepath);
    write_header();
  }

  void end_session() {
    write_footer();

    output_stream_.close();
    profile_count_ = 0;
  }

  void write_profile(Profile_result&& result) {
    if (profile_count_++ > 0)
        output_stream_ << ",\n";

    std::string name = result.name;
    std::replace(name.begin(), name.end(), '"', '\'');

    output_stream_ << "  {";
    output_stream_ << " \"name\": \"" << name << "\",";
    output_stream_ << " \"start\": " << result.start << ",";
    output_stream_ << " \"dur\": " << result.duration << ",";
    output_stream_ << " \"tid\": " << result.thread_num << " }";
    output_stream_.flush();
  }

  static Instrumentor& get() {
    static Instrumentor instance;
    return instance;
  }

 private:
  std::ofstream output_stream_;
  int profile_count_;

  void write_header() {
    output_stream_ << "{ \"record\": [\n";
    output_stream_.flush();
  }

  void write_footer() {
    output_stream_ << "\n]}";
    output_stream_.flush();
  }
};

class Instrumentation_timer {
 public:
  Instrumentation_timer(const char* name)
    : name_(name), stopped_(false) {
    start_ = std::chrono::steady_clock::now();
  }

  ~Instrumentation_timer() {
    if (!stopped_)
     stop();
  }

  void stop() {
    auto end = std::chrono::steady_clock::now();

    auto start = std::chrono::duration_cast<
      std::chrono::microseconds>(start_.time_since_epoch());

    auto elapsed = std::chrono::duration_cast<
      std::chrono::microseconds>(end - start_);

    #pragma omp critical
    {
      int thread_num = omp_get_thread_num();
      Instrumentor::get().write_profile({
        name_, start.count(), elapsed.count(), thread_num
      });
    }

    stopped_ = true;
  }

 private:
  const char* name_;
  std::chrono::time_point<std::chrono::steady_clock> start_;
  bool stopped_;
};

#if TIME_PROFILING
#define BEGIN_SESSION(filepath)  ::Instrumentor::get().begin_session(filepath)
#define END_SESSION()            ::Instrumentor::get().end_session()
#define PROFILE_SCOPE(name)      ::Instrumentation_timer timer##__LINE__(name)
#define PROFILE_FUNCTION()       PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
#define BEGIN_SESSION(filepath)
#define END_SESSION()
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif

#endif  // SRC_MANAGERS_TIME_MANAGER_HPP

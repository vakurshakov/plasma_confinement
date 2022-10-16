#include "time_manager.hpp"

Instrumentor::Instrumentor()
  : profile_count_(0) {}

void Instrumentor::begin_session(const std::string& filepath) {
  output_stream_.open(filepath);
  write_header();
}

void Instrumentor::end_session() {
  write_footer();

  output_stream_.close();
  profile_count_ = 0;
}

void Instrumentor::write_profile(Profile_result&& result) {
  if (profile_count_++ > 0)
      output_stream_ << ",\n";

  std::string name = result.name;
  std::replace(name.begin(), name.end(), '"', '\'');

  output_stream_ << "  {\n";
  output_stream_ << "    \"name\": \"" << name << "\",\n";
  output_stream_ << "    \"start\": " << result.start << ",\n";
  output_stream_ << "    \"end\": " << result.end << ",\n";
  output_stream_ << "    \"dur\": " << result.duration << ",\n";
  output_stream_ << "    \"tid\": " << result.thread_num << "\n  }";
  output_stream_.flush();
}

/* static */ Instrumentor& Instrumentor::get() {
  static Instrumentor instance;
  return instance;
}

void Instrumentor::write_header() {
  output_stream_ << "{ \"record\": [\n";
  output_stream_.flush();
}

void Instrumentor::write_footer() {
  output_stream_ << "\n]}";
  output_stream_.flush();
}


Instrumentation_timer::Instrumentation_timer(const char* name)
  : name_(name), stopped_(false) {
  start_ = std::chrono::system_clock::now();
}

Instrumentation_timer::~Instrumentation_timer() {
  if (!stopped_)
   stop();
}

void Instrumentation_timer::stop() {
  #pragma omp master
  {
    const auto end = std::chrono::system_clock::now();

    const auto elapsed = duration_cast<
      std::chrono::microseconds>(end - start_);

    int thread_num = omp_get_thread_num();

    Instrumentor::get().write_profile({
      name_,
      std::to_string(start_.time_since_epoch().count() / 1e9),  // from ns
      std::to_string(end.time_since_epoch().count() / 1e9),  // from ns
      elapsed.count(),
      thread_num
    });
  }

  stopped_ = true;
}


Accumulative_timer::Accumulative_timer(const char* name)
  : name_(name) {}

void Accumulative_timer::start(int total) {
  if (current_ == 0)
    set_new(total);

  current_++;
  start_ = std::chrono::system_clock::now();
}

void Accumulative_timer::set_new(int total) {
  /// @todo Here is only one problem. If total is less then
  /// num_threads, timer will always show zero duration.
  total_ = total / omp_get_num_threads();

  first_ = std::chrono::system_clock::now();
  accumulated_duration = accumulated_duration.zero();
}

void Accumulative_timer::stop() {
  const auto now = std::chrono::system_clock::now();

  accumulated_duration += std::chrono::duration_cast<
    std::chrono::microseconds>(now - start_);

  if (current_ >= total_)
    drop_and_reset();
}

void Accumulative_timer::drop_and_reset() {
  #pragma omp master
  {
    const auto end = std::chrono::system_clock::now();

    int thread_num = omp_get_thread_num();

    Instrumentor::get().write_profile({
      name_,
      std::to_string(first_.time_since_epoch().count() / 1e9),  // from ns
      std::to_string(end.time_since_epoch().count() / 1e9),  // from ns
      accumulated_duration.count(),
      thread_num
    });
  }

  current_ = 0;
}
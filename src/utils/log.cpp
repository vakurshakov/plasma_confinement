#include "log.hpp"

#include "spdlog/sinks/basic_file_sink.h"

std::shared_ptr<spdlog::logger> Log::logger_;

void Log::Init(const char* filename) {
  // basic logger that will write everything to simulation.log
  logger_ = spdlog::basic_logger_mt("Plasma_simulation", filename, /* truncate = */ true);
  
  logger_->set_pattern("%^[%D %T]: %v %$");
  
  logger_->set_level(spdlog::level::trace);
  logger_->flush_on(spdlog::level::warn);
}

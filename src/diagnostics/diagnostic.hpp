#ifndef SRC_DIAGNOSTICS_DIAGNOSTIC_HPP
#define SRC_DIAGNOSTICS_DIAGNOSTIC_HPP

#include "src/pch.h"
#include "src/file_writers/file_interface.hpp"

/**
 * @brief Diagnostic interface. Its child elements must
 * specify the data to diagnose as fields and process it
 * by overriding the `diagnose` method.
 *
 * @todo Take a look at OpenPMD.
 */
class Diagnostic {
 public:
  virtual ~Diagnostic() = default;

  Diagnostic(std::string result_directory)
    : result_directory_(result_directory) {};

  virtual void diagnose(int t) = 0;

  virtual void save_parameters() const = 0;

 protected:
  std::string result_directory_;
  std::unique_ptr<IFile> file_for_results_ = nullptr;
};

#endif  // SRC_DIAGNOSTICS_DIAGNOSTIC_HPP

#include <omp.h>

#include <random>

/**
 * @brief This is a singleton around the linear congruential
 *      engine (namely the minstd) for random numbers.
 *
 * @see https://en.cppreference.com/w/cpp/numeric/random
 * @see https://en.cppreference.com/w/cpp/numeric/random/linear_congruential_engine
 *
 */
class Random_generator {
 public:

  static inline std::minstd_rand& get() {
    static Random_generator single_instance;
    return single_instance.gen;
  }

 private:
  std::random_device rd;
  std::minstd_rand gen = std::minstd_rand(rd());

  Random_generator() = default;

  Random_generator(const Random_generator&) = delete;
  Random_generator& operator=(const Random_generator&) = delete;

  Random_generator(Random_generator&&) = delete;
  Random_generator& operator=(Random_generator&&) = delete;
};

inline double random_01() {
  static std::uniform_real_distribution distribution(0.0, 1.0);

  double value;

  /// @todo critical section can slow everything
  /// down, try here: #pragma omp atomic
  #pragma omp critical
  value = distribution(Random_generator::get());

  return value;
}

inline int random_sign() { 
  static std::bernoulli_distribution distribution(0.5);

  int value;

  #pragma omp critical
  value = distribution(Random_generator::get()) ? +1 : -1;

  return value;
}
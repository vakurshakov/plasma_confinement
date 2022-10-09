#include <random>

/**
 * @brief This is a singleton around the linear congruential
 *      engine (namely the minstd) for random numbers.
 * 
 * @see https://en.cppreference.com/w/cpp/numeric/random
 * @see https://en.cppreference.com/w/cpp/numeric/random/linear_congruential_engine
 *
 * @warning This is a random number generator, it won't
 *      generate a concrete distribution. To do so, use the
 *      random number distribution from <random> instead.
 * 
 * @todo Do the singleton thread-safe.
 */
class Random_generator {
public:

    /**
     * @brief Gets instance of the generator.
     * @return The single instance of a PRNG.
     */
    static auto& get()
    {
        static Random_generator single_instance;
        return single_instance.gen;
    }

private:
    std::minstd_rand gen;
    
    Random_generator() = default;
    
    Random_generator(const Random_generator& a) = delete;
    Random_generator& operator=(const Random_generator& a) = delete;
    
    Random_generator(Random_generator&&) = delete;
    Random_generator& operator=(Random_generator&&) = delete;
};

inline double random_01() {
  static auto distribution = std::uniform_real_distribution(0., std::nextafter(1., 2.));
  return distribution(Random_generator::get());
}
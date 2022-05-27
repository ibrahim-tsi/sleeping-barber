#include <random>

#include "util.hpp"

namespace util {
  int rnd_range(int min, int max) { // both inclusive
    static std::random_device dev{};
    std::mt19937 rng{dev()};
    std::uniform_int_distribution<int> dist{min, max};

    return dist(rng);
  }

  bool rnd_bool() { return rnd_range(0, 1); };
} 
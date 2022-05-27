#pragma once

#include <cstdint>
#include <functional>
#include <random>

#define ARR_SIZE(a) (sizeof(a) / sizeof(a[0]))

namespace util {
  int rnd_range(int min, int max);
  bool rnd_bool();
  
  template <typename T>
  T rnd_enum_class() {
    return (T)util::rnd_range(0, (int)T::end - 1);
  }

  template <class T>
  inline void hash_combine(std::size_t& seed, const T& v) { // boost::hash_combine
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
  }
}
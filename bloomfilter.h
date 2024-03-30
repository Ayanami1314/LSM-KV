
#ifndef __BF__H
#define __BF__H
#include "MurmurHash3.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <numeric>
#include <vector>

class BloomFilter {
private:
  std::vector<bool> BF;
  std::vector<
      std::function<void(const void *, const int, const uint32_t, void *)>>
      hashes;
  int hash_gen_seed;

public:
  BloomFilter(size_t length, int hash_func_number, int seed = 13147);
  bool find_u64(const uint64_t key) const;
  void insert_u64(const uint64_t key);
  void clear() { std::fill(BF.begin(), BF.end(), false); }
};

#endif
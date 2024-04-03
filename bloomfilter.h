
#ifndef __BF__H
#define __BF__H
#include "MurmurHash3.h"
#include "type.h"
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
  BloomFilter(size_t length, int hash_func_number = 3, int seed = 13147);
  BloomFilter(const TBytes &bytes, int hash_func_number = 3, int seed = 13147);
  BloomFilter(const BloomFilter &other);
  [[nodiscard]] bool find_u64(uint64_t key) const;
  void insert_u64(uint64_t key);
  void clear() { std::fill(BF.begin(), BF.end(), false); }
  [[nodiscard]] std::vector<bool> get() const { return this->BF; }
  [[nodiscard]] std::vector<uint8_t> toBytes() const;
};

#endif
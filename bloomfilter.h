
#ifndef __BF__H
#define __BF__H
#include "MurmurHash3.h"
#include "type.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>
class BloomFilter {
private:
  std::vector<bool> BF;
  std::vector<
      std::function<void(const void *, const int, const uint32_t, void *)>>
      hashes;
  int hash_gen_seed;

public:
  static const int default_hash_gen_seed = 13147;
  BloomFilter(size_t length, int hash_func_number = 3,
              int seed = default_hash_gen_seed);
  BloomFilter(const TBytes &bytes, int hash_func_number = 3,
              int seed = default_hash_gen_seed);
  BloomFilter(const BloomFilter &other);
  [[nodiscard]] bool find_u64(uint64_t key) const;
  void insert_u64(uint64_t key);
  void clear() { std::fill(BF.begin(), BF.end(), false); }
  void print() {
    for (int i = 0; i < BF.size(); ++i) {
      if (BF[i]) {
        std::cout << i << " ";
      }
    }
    std::cout << std::endl;
  }
  [[nodiscard]] int getSeed() const { return this->hash_gen_seed; }
  [[nodiscard]] int getHashFuncNum() const { return this->hashes.size(); }
  [[nodiscard]] std::vector<bool> get() const { return this->BF; }
  [[nodiscard]] std::vector<uint8_t> toBytes() const;
};

#endif
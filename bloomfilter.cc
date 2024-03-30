#include "bloomfilter.h"

BloomFilter::BloomFilter(size_t length, int hash_func_number, int seed)
    : hash_gen_seed(seed) {

  BF.resize(length, false);
  hashes.resize(hash_func_number);
  for (int i = 0; i < hash_func_number; i++) {
    hashes[i] = [i, this](const void *key, const int len, const uint32_t seed,
                          void *out) -> void {
      int key_val = *(static_cast<const int *>(key)) + i * hash_gen_seed;
      MurmurHash3_x64_128(&key_val, sizeof(key_val), seed, out);
    };
  }
}
void BloomFilter::insert_u64(const uint64_t key) {
  std::vector<int> results;
  for (const auto &h : hashes) {
    uint64_t hash_out[2] = {0};
    h(&key, sizeof(key), hash_gen_seed, hash_out);
    results.push_back(hash_out[0] % BF.size());
  }
  for_each(results.begin(), results.end(),
           [this](int i) { this->BF.at(i) = true; });
}

bool BloomFilter::find_u64(const uint64_t key) const {
  std::vector<int> results;
  for (const auto &h : hashes) {
    uint64_t hash_out[2] = {0};
    h(&key, sizeof(key), hash_gen_seed, hash_out);
    results.push_back(hash_out[0] % BF.size());
  }
  bool exist = true;
  for (const auto &i : results) {
    if (!this->BF.at(i)) {
      exist = false;
      break;
    }
  }
  return exist;
}
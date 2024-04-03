#include "bloomfilter.h"

BloomFilter::BloomFilter(size_t length, int hash_func_number, int seed)
    : hash_gen_seed(seed) {
  // HINT
  BF.resize(length, true);
  hashes.resize(hash_func_number);
  for (int i = 0; i < hash_func_number; i++) {
    hashes[i] = [i, this](const void *key, const int len, const uint32_t seed,
                          void *out) -> void {
      int key_val = *(static_cast<const int *>(key)) + i * hash_gen_seed;
      MurmurHash3_x64_128(&key_val, sizeof(key_val), seed, out);
    };
  }
}

BloomFilter::BloomFilter(const TBytes &bytes, int hash_func_number, int seed)
    : hash_gen_seed(seed) {
  std::copy(bytes.begin(), bytes.end(), std::back_inserter(BF));
  hashes.resize(hash_func_number);
  for (int i = 0; i < hash_func_number; i++) {
    hashes[i] = [i, this](const void *key, const int len, const uint32_t seed,
                          void *out) -> void {
      int key_val = *(static_cast<const int *>(key)) + i * hash_gen_seed;
      MurmurHash3_x64_128(&key_val, sizeof(key_val), seed, out);
    };
  }
}
BloomFilter::BloomFilter(const BloomFilter &other)
    : hash_gen_seed(other.hash_gen_seed), BF(other.BF), hashes(other.hashes) {}
void BloomFilter::insert_u64(uint64_t key) {
  std::vector<int> results;
  for (const auto &h : hashes) {
    uint64_t hash_out[2] = {0};
    h(&key, sizeof(key), hash_gen_seed, hash_out);
    results.push_back(hash_out[0] % BF.size());
  }
  for_each(results.begin(), results.end(),
           [this](int i) { this->BF.at(i) = true; });
}

bool BloomFilter::find_u64(uint64_t key) const {
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

std::vector<uint8_t> BloomFilter::toBytes() const {
  std::vector<uint8_t> bytes;
  uint8_t byte = 0;
  if (this->BF.size() % 8 != 0) {
    // NOTE: the size must be the multiple of 8
    throw("BF size is not a multiple of 8");
  }
  for (size_t i = 0; i < this->BF.size(); ++i) {
    byte = (byte << 1) | this->BF[i];
    if ((i + 1) % 8 == 0) {
      bytes.push_back(byte);
      byte = 0;
    }
  }
  // Handle the last byte if the size of BF is not a multiple of 8
  if (this->BF.size() % 8 != 0) {
    byte <<= (8 - this->BF.size() % 8); // Shift remaining bits
    bytes.push_back(byte);
  }
  return bytes;
}

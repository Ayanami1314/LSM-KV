#include "bloomfilter.h"
#include "MurmurHash3.h"
BloomFilter::BloomFilter(size_t length, int hash_func_number, int seed)
    : hash_gen_seed(seed) {
  // HINT
  BF.resize(length, true);
  hashes.resize(hash_func_number);
  auto gen_seed = this->hash_gen_seed;
  for (int i = 0; i < hash_func_number; i++) {
    hashes[i] = [i, gen_seed](const void *key, const int len,
                              const uint32_t seed, void *out) -> void {
      int key_val = *(static_cast<const int *>(key)) + i * gen_seed;
      MurmurHash3_x64_128(&key_val, sizeof(key_val), seed, out);
    };
  }
}

BloomFilter::BloomFilter(const TBytes &bytes, int hash_func_number, int seed)
    : hash_gen_seed(seed) {
  for (auto byte : bytes) {
    for (int i = 7; i >= 0; --i) {
      BF.push_back((byte >> i) & 1);
    }
  }
  hashes.resize(hash_func_number);
  auto gen_seed = this->hash_gen_seed;
  for (int i = 0; i < hash_func_number; i++) {
    hashes[i] = [i, gen_seed](const void *key, const int len,
                              const uint32_t seed, void *out) -> void {
      int key_val = *(static_cast<const int *>(key)) + i * gen_seed;
      MurmurHash3_x64_128(&key_val, sizeof(key_val), seed, out);
    };
  }
}
BloomFilter::BloomFilter(const BloomFilter &other)
    : BF(other.BF), hash_gen_seed(other.hash_gen_seed) {
  hashes.resize(other.hashes.size());
  int size = other.hashes.size();
  auto gen_seed = this->hash_gen_seed;
  for (int i = 0; i < size; i++) {
    hashes[i] = [i, gen_seed](const void *key, const int len,
                              const uint32_t seed, void *out) -> void {
      int key_val = *static_cast<const int *>(key) + i * gen_seed;
      MurmurHash3_x64_128(&key_val, sizeof(key_val), seed, out);
    };
  }
}
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
/**
@brief find if the u64 key exist
 * @param  key
 * @return true
 * @return false
 */
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
/**
@brief serialize the BF
 * @return std::vector<uint8_t>
 */
std::vector<uint8_t> BloomFilter::toBytes() const {
  std::vector<uint8_t> bytes;
  uint8_t byte = 0;
  size_t size = this->BF.size();
  if (size % 8 != 0) {
    // NOTE: the size must be the multiple of 8
    throw("BF size is not a multiple of 8");
  }
  // byte: HIGH 01234567 LOW
  for (size_t i = 0; i < size; i += 8) {
    for (int j = 0; j < 8; ++j) {
      byte = (byte << 1) | this->BF[i + j];
    }
    bytes.push_back(byte);
    byte = 0;
  }

  return bytes;
}

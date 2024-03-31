
#ifndef __SSTABLE_H
#define __SSTABLE_H
#include "bloomfilter.h"
#include "type.h"
#include <cstddef>

namespace SSTable {

class Header {
  u64 timeStamp;
  u64 num_of_kv;
  u64 minKey;
  u64 maxKey;

  // getters
public:
  Header(u64 timeStamp_, u64 num_of_kv_, u64 minKey_, u64 maxKey_)
      : timeStamp(timeStamp_), num_of_kv(num_of_kv_), minKey(minKey_),
        maxKey(maxKey_) {}
  Header() : timeStamp(0), num_of_kv(0), minKey(0), maxKey(0) {}
  [[nodiscard]] u64 getTimeStamp() const { return timeStamp; }
  [[nodiscard]] u64 getNumOfKV() const { return num_of_kv; }
  [[nodiscard]] u64 getMinKey() const { return minKey; }
  [[nodiscard]] u64 getMaxKey() const { return maxKey; }
};

class sstable_type {
private:
  size_t bf_size;
  int hash_func_num;
  u64 ss_uid;
  static u64 ss_total_uid;
  static const size_t default_bf_size = 8 * 1024 * 8; // 8KB = 8*8*1024 bit
  BloomFilter BF;
  Header header;
  kEntrys kes;

public:
  sstable_type(size_t BF_size = default_bf_size, int hash_num = 3);
  sstable_type(const kEntrys &kes, size_t BF_size = default_bf_size,
               int hash_num = 3);
  size_t size();
  static size_t cal_size(int number_of_kv, size_t BF_size = default_bf_size);
  void save(const std::string &path);
  void load(const std::string &path);

  ~sstable_type() = default;
  u64 getUID() const { return ss_uid; }
};
} // namespace SSTable
#endif
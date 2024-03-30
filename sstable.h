
#ifndef __SSTABLE_H
#define __SSTABLE_H
#include "bloomfilter.h"
#include "type.h"
#include <cstddef>

namespace SSTable {
typedef struct header {
  u64 timeStamp;
  u64 num_of_kv;
  u64 minKey;
  u64 maxKey;
  header(u64 timeStamp_, u64 num_of_kv_, u64 minKey_, u64 maxKey_)
      : timeStamp(timeStamp_), num_of_kv(num_of_kv_), minKey(minKey_),
        maxKey(maxKey_) {}
} Header;

class sstable_type {
private:
  size_t bf_size;
  int hash_func_num;
  u64 ss_uid;
  static u64 ss_total_uid;
  static const size_t default_bf_size = 8 * 1024 * 8; // 8KB = 8*8*1024 bit

public:
  BloomFilter BF;
  Header header;
  kEntrys kes;
  sstable_type(size_t BF_size = default_bf_size, int hash_num = 3);
  size_t size();
  static size_t cal_size(int number_of_kv, size_t BF_size = default_bf_size);
};
} // namespace SSTable
#endif
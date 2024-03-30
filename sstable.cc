#include "sstable.h"
#include "bloomfilter.h"
#include <cassert>

namespace SSTable {
u64 sstable_type::ss_total_uid = 1; // the first timestamp is 1
sstable_type::sstable_type(size_t BF_size, int hash_num)
    : ss_uid(ss_total_uid), bf_size(BF_size), hash_func_num(hash_num),
      BF(BF_size, hash_num), header({ss_total_uid, 0, 0, 0}) {
  // constructor implementation
  ss_total_uid++;
}

size_t sstable_type::size() {
  // num_of_kv: u64
  return header.num_of_kv * sizeof(u64) + sizeof(header) + bf_size / 8;
}

size_t sstable_type::cal_size(int number_of_kv, size_t BF_size) {
  return number_of_kv * sizeof(u64) + sizeof(header) + BF_size / 8;
}
} // namespace SSTable
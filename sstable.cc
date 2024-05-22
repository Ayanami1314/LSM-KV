#include "sstable.h"
#include "bloomfilter.h"
#include "type.h"
#include "utils.h"
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
namespace SSTable {
u64 sstable_type::ss_total_uid = 1; // the first timestamp is 1
void sstable_type::resetID() { ss_total_uid = 1; }
sstable_type::sstable_type(u64 BF_size, int hash_num)
    : ss_uid(ss_total_uid), bf_size(BF_size), hash_func_num(hash_num),
      BF(BF_size, hash_num), header({ss_total_uid, 0, 0, 0}) {
  // constructor implementation
  ss_total_uid++;
}
sstable_type::sstable_type(const kEntrys &kes, u64 timeStamp, u64 BF_size,
                           int hash_num)
    : ss_uid(ss_total_uid), bf_size(BF_size), hash_func_num(hash_num),
      BF(BF_size, hash_num), header({ss_total_uid, 0, 0, 0}),
      pkes(std::make_shared<kEntrys>(kes)) {
  // constructor implementation
  // ss_total_uid++;
  // gen BF
  TKey minKey = std::numeric_limits<uint64_t>::max();
  TKey maxKey = 0x0;
  u64 num_of_kv = 0x0;
  for (const auto &entry : kes) {
    BF.insert_u64(entry.key);
    if (entry.key > maxKey) {
      maxKey = entry.key;
    }
    if (entry.key < minKey) {
      minKey = entry.key;
    }
    num_of_kv++;
  }
  header.setMinKey(minKey);
  header.setMaxKey(maxKey);
  header.setNumOfKV(num_of_kv);
  header.setTimeStamp(timeStamp);
}
sstable_type::sstable_type(kEntrys &&kes, u64 timeStamp, u64 BF_size,
                           int hash_num)
    : ss_uid(ss_total_uid), bf_size(BF_size), hash_func_num(hash_num),
      BF(BF_size, hash_num), header({ss_total_uid, 0, 0, 0}),
      pkes(std::make_shared<kEntrys>(kes)) {
  // constructor implementation
  // ss_total_uid++;
  // gen BF
  TKey minKey = std::numeric_limits<u64>::max();
  TKey maxKey = std::numeric_limits<u64>::min();
  u64 num_of_kv = std::numeric_limits<u64>::min();
  for (auto &entry : kes) {
    BF.insert_u64(entry.key);
    if (entry.key > maxKey) {
      maxKey = entry.key;
    }
    if (entry.key < minKey) {
      minKey = entry.key;
    }
    num_of_kv++;
  }
  header.setMinKey(minKey);
  header.setMaxKey(maxKey);
  header.setNumOfKV(num_of_kv);
}
sstable_type::sstable_type(const sstable_type &other)
    : ss_uid(other.ss_uid), bf_size(other.bf_size),
      hash_func_num(other.hash_func_num), BF(other.BF), header(other.header),
      pkes(other.pkes) {}
void sstable_type::addBF(const kEntrys &kes) {
  if (!config::use_bf || !config::use_cache) [[unlikely]] {
    throw("Try to add BF when config::use_bf is false");
  }
  for (auto &entry : *pkes) {
    BF.insert_u64(entry.key);
  }
}
/**
@brief
 * @return u64 the generated file size of the current sstable
 */
u64 sstable_type::size() const {
  // num_of_kv: u64
  return header.getNumOfKV() * sizeof(u64) + sizeof(header) + bf_size / 8;
}
void sstable_type::clear() {
  pkes = std::make_unique<kEntrys>();
  header.clear();
  if (config::use_bf) {
    BF.clear();
    bf_size = config::bf_default_size;
  }
}
/**
@brief calculate the generated file size of the current sstable
 * @param  number_of_kv
 * @param  BF_size
 * @return u64
 */
u64 sstable_type::cal_size(int number_of_kv, u64 BF_size) {
  return number_of_kv * sizeof(kEntry) + sizeof(header) + BF_size / 8;
}

/**
@brief judge if the key exist by Header and BF
 * @param  key
 * @return true
 * @return false
 */
bool sstable_type::mayKeyExist(TKey key) const {
  if (key > header.getMaxKey() || key < header.getMinKey()) {
    return false;
  }
  if (config::use_bf && !BF.find_u64(key)) {
    return false;
  }
  return true;
}
// bool sstable_type::mayKeyExist(TKey key, std::string ss_file) const {
//   std::ifstream ifs(ss_file);
//   if (!ifs.is_open()) {
//     throw("Try to get size of a file that not exists");
//   }
//   Header file_header;
//   ifs.read(reinterpret_cast<char *>(&file_header), sizeof(file_header));
//   ifs.close();
//   if (key > file_header.getMaxKey() || key < file_header.getMinKey()) {
//     return false;
//   }
//   return true;
// }
u64 binary_search_helper(TKey key, u64 left, u64 right, const kEntrys &kes,
                         bool &found) {
  if (left == right) {
    found = kes[left].key == key;
    return found ? left : -1;
  }
  u64 mid = (left + right) / 2;
  if (kes[mid].key == key) {
    found = true;
    return mid;
  }
  if (kes[mid].key > key) {
    return binary_search_helper(key, left, mid, kes, found);
  }
  return binary_search_helper(key, mid + 1, right, kes, found);
}
/**
@brief binary search in sstable in range [0, total)
 * @param  key
 * @param  total total number of kv
 * @param  exist true if found, false if not found
 * @param  use_BF
 * @return u64 index if found, -1 if not found
 */
u64 sstable_type::binary_search(TKey key, u64 total, bool &exist,
                                bool use_BF) const {
  // NOTE: idx form 0 to total - 1
  if (use_BF && !BF.find_u64(key)) {
    exist = false;
    return -1;
  }
  exist = false;
  return binary_search_helper(key, 0, total - 1, *pkes, exist);
}
/**
@brief return the kEntry by key
 * @param  key
 * @return ke_not_found macro if not found
 */
kEntry sstable_type::query(TKey key) const {
  if (!config::use_cache) {
    throw("Try to query in cache when config::use_cache is false");
  }
  if (config::use_bf) {
    if (!mayKeyExist(key)) {
      return type::ke_not_found;
    }
  }

  // for (auto &entry : *pkes) {
  //   if (entry.key == key) {
  //     return entry;
  //   }
  // }
  bool exist = false;
  auto total = header.getNumOfKV();
  auto choose = binary_search(key, total, exist, false);
  if (!exist) {
    return type::ke_not_found;
  }

  Log("The total number of kv is %d", total);
  Log("The real key is %llu", key);

  if (pkes->at(choose).key != key) {
    return type::ke_not_found;
  }

  Log("The choose key is %llu", choose);
  return pkes->at(choose);
}
/**
@brief scan the sstable
 * @param  min minKey
 * @param  max maxKey
 * @param  res a list of kEntry(key, offset, vlen)
 */
void sstable_type::scan(TKey min, TKey max, kEntrys &res) const {
  if (min > header.getMaxKey() || max < header.getMinKey()) {
    return;
  }
  if (min < header.getMinKey()) {
    min = header.getMinKey();
  }
  if (max > header.getMaxKey()) {
    max = header.getMaxKey();
  }
  for (const auto &entry : *pkes) {
    if (entry.key >= min && entry.key <= max) {
      res.push_back(entry);
    }
  }
  return;
}
/**
@brief save the SSTable to path(will not clear the current sstable)
 * @param  path
 */
void sstable_type::save(const std::string &path) {
  if (std::filesystem::exists(path)) {
    std::string msg = "Try to save to file(%s) that already exists";
    Log(msg, path.c_str());
  }
  std::ofstream ofile(path, std::ios::binary | std::ios::trunc);
  // | Header | BF | kEntrys |
  ofile.write(reinterpret_cast<char *>(&header), sizeof(header));
  if (config::use_bf) {
    auto bytes = BF.toBytes();
    ofile.write(reinterpret_cast<char *>(bytes.data()), bytes.size());
  }

  for (auto &entry : *pkes) {
    ofile.write(reinterpret_cast<char *>(&entry), sizeof(entry));
  }
  ofile.flush(); // ATTENTION: 立即刷盘
  ofile.close();
}

void sstable_type::load(const std::string &path) {
  std::ifstream ifile(path, std::ios::binary);
  if (!std::filesystem::exists(path)) {
    std::string msg = "Try to load file(%s) that not exists";
    Log(msg, path.c_str());
  }
  ifile.read(reinterpret_cast<char *>(&header), sizeof(header));
  if (config::use_bf) {
    TBytes bytes(bf_size / 8);
    ifile.read(reinterpret_cast<char *>(bytes.data()), bytes.size());

    this->BF = BloomFilter(bytes);
    assert(BF.default_hash_gen_seed == BF.getSeed());
  }

  std::list<kEntry> kes;
  for (int i = 0; i < header.getNumOfKV(); ++i) {
    kEntry entry;
    ifile.read(reinterpret_cast<char *>(&entry), sizeof(entry));
    kes.push_back(entry);
  }
  pkes = std::make_shared<kEntrys>(std::make_move_iterator(kes.begin()),
                                   std::make_move_iterator(kes.end()));
  ifile.close();
}
} // namespace SSTable

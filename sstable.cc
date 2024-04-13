#include "sstable.h"
#include "bloomfilter.h"
#include "type.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <iterator>
namespace SSTable {
u64 sstable_type::ss_total_uid = 1; // the first timestamp is 1
void sstable_type::resetID() { ss_total_uid = 1; }
sstable_type::sstable_type(size_t BF_size, int hash_num)
    : ss_uid(ss_total_uid), bf_size(BF_size), hash_func_num(hash_num),
      BF(BF_size, hash_num), header({ss_total_uid, 0, 0, 0}) {
  // constructor implementation
  ss_total_uid++;
}
sstable_type::sstable_type(const kEntrys &kes, size_t BF_size, int hash_num)
    : ss_uid(ss_total_uid), bf_size(BF_size), hash_func_num(hash_num),
      BF(BF_size, hash_num), header({ss_total_uid, 0, 0, 0}),
      pkes(std::make_shared<kEntrys>(kes)) {
  // constructor implementation
  ss_total_uid++;
  // gen BF
  TKey minKey = 0xffffffffffffffff;
  TKey maxKey = 0x0;
  u64 num_of_kv = 0x0;
  for (auto &entry : *(pkes.get())) {
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
    : ss_uid(other.ss_uid), BF(other.BF), header(other.header),
      pkes(other.pkes), bf_size(other.bf_size),
      hash_func_num(other.hash_func_num) {}
void sstable_type::addBF(const kEntrys &kes) {
  for (auto &entry : *pkes) {
    BF.insert_u64(entry.key);
  }
}
/**
@brief
 * @return size_t the generated file size of the current sstable
 */
size_t sstable_type::size() const {
  // num_of_kv: u64
  return header.getNumOfKV() * sizeof(u64) + sizeof(header) + bf_size / 8;
}
/**
@brief clear the current sstable
 */
void sstable_type::clear() {
  pkes = std::make_unique<kEntrys>();
  BF.clear();
  header.clear();
  bf_size = default_bf_size;
}
/**
@brief calculate the generated file size of the current sstable
 * @param  number_of_kv
 * @param  BF_size
 * @return size_t
 */
size_t sstable_type::cal_size(int number_of_kv, size_t BF_size) {
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
  if (!BF.find_u64(key)) {
    return false;
  }
  return true;
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
  u64 left = 0;
  u64 right = total - 1;
  u64 choose = 0;
  exist = true;
  while (left != right) {
    u64 mid = (left + right) / 2;
    if (key == pkes->at(left).key) {
      choose = left;
      break;
    }
    if (key == pkes->at(right).key) {
      choose = right;
      break;
    }
    if (key == pkes->at(mid).key) {
      choose = mid;
      break;
    }
    if (pkes->at(left).key < key && key < pkes->at(mid).key) {
      right = mid;
      continue;
    }
    // BUG here: always left = mid
    if (key > pkes->at(mid).key && key < pkes->at(right).key) {
      if (left == mid) {
        exist = false;
        return -1;
      }
      left = mid;
      continue;
    }
  }
  if (left == right) {
    choose = left;
  }
  return choose;
}
/**
@brief return the kEntry by key
 * @param  key
 * @return ke_not_found macro if not found, ke_deleted macro if deleted
 */
kEntry sstable_type::query(TKey key) const {
  if (!mayKeyExist(key)) {
    return type::ke_not_found;
  }
  // for (auto &entry : *pkes) {
  //   if (entry.key == key) {
  //     return entry;
  //   }
  // }
  bool exist = false;
  auto choose = binary_search(key, header.getNumOfKV(), exist, false);
  if (!exist) {
    return type::ke_not_found;
  }
  auto total = header.getNumOfKV();
  Log("The total number of kv is %d", total);
  Log("The real key is %llu", key);

  if (pkes->at(choose).key != key) {
    return type::ke_not_found;
  }

  Log("The choose key is %llu", choose);
  if (pkes->at(choose).len == 0) {
    return type::ke_deleted;
  }
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
  bool exist_min = false;
  bool exist_max = false;
  auto min_in_this = binary_search(min, header.getNumOfKV(), exist_min);
  auto max_in_this = binary_search(max, header.getNumOfKV(), exist_max);
  if (exist_min) {
    Log("min_in_this is %llu", min_in_this);
  }
  if (exist_max) {
    Log("max_in_this is %llu", max_in_this);
  }
  if (!exist_min && !exist_max) {
    return;
  }
  if (exist_min && exist_max) {
    for (auto i = min_in_this; i <= max_in_this; ++i) {
      res.push_back(pkes->at(i));
    }
  }
  if (exist_min && !exist_max) {
    for (auto i = min_in_this; i < header.getNumOfKV(); ++i) {
      res.push_back(pkes->at(i));
    }
  }
  if (!exist_min && exist_max) {
    for (auto i = 0; i <= max_in_this; ++i) {
      res.push_back(pkes->at(i));
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
    Log(msg, path);
  }
  std::ofstream ofile(path, std::ios::binary | std::ios::trunc);
  // | Header | BF | kEntrys |
  ofile.write(reinterpret_cast<char *>(&header), sizeof(header));
  auto bytes = BF.toBytes();
  ofile.write(reinterpret_cast<char *>(bytes.data()), bytes.size());

  for (auto &entry : *pkes) {
    ofile.write(reinterpret_cast<char *>(&entry), sizeof(entry));
  }
  ofile.close();
}

/**
@brief load a sstable from path
 * @param  path
 */
void sstable_type::load(const std::string &path) {
  std::ifstream ifile(path, std::ios::binary);
  if (!std::filesystem::exists(path)) {
    std::string msg = "Try to load file(%s) that not exists";
    Log(msg, path);
  }
  ifile.read(reinterpret_cast<char *>(&header), sizeof(header));
  TBytes bytes(bf_size / 8);
  ifile.read(reinterpret_cast<char *>(bytes.data()), bytes.size());

  BF = BloomFilter(bytes);

  std::list<kEntry> kes;
  for (int i = 0; i < header.getNumOfKV(); ++i) {
    kEntry entry;
    ifile.read(reinterpret_cast<char *>(&entry), sizeof(entry));
    kes.push_back(std::move(entry));
  }
  pkes = std::make_shared<kEntrys>(std::make_move_iterator(kes.begin()),
                                   std::make_move_iterator(kes.end()));
  ifile.close();
}
} // namespace SSTable

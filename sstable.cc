#include "sstable.h"
#include "bloomfilter.h"
#include "type.h"
#include "utils.h"
#include <fstream>
#include <iostream>
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
  for (auto &entry : *pkes) {
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
size_t sstable_type::size() {
  // num_of_kv: u64
  return header.getNumOfKV() * sizeof(u64) + sizeof(header) + bf_size / 8;
}
void sstable_type::clear() {
  pkes = std::make_unique<kEntrys>();
  BF.clear();
  header.clear();
}
size_t sstable_type::cal_size(int number_of_kv, size_t BF_size) {
  return number_of_kv * sizeof(kEntry) + sizeof(header) + BF_size / 8;
}
bool sstable_type::mayKeyExist(TKey key) {
  if (key > header.getMaxKey() || key < header.getMinKey()) {
    return false;
  }
  if (!BF.find_u64(key)) {
    return false;
  }
  return true;
}

kEntry sstable_type::query(TKey key) {
  if (!mayKeyExist(key)) {
    return type::ke_not_found;
  }
  // for (auto &entry : *pkes) {
  //   if (entry.key == key) {
  //     return entry;
  //   }
  // }
  auto total = header.getNumOfKV();
  Log("The total number of kv is %d", total);
  Log("The real key is %llu", key);
  u64 left = 0;
  u64 right = total - 1;
  u64 choose = 0;
  bool exist = true;
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
    if (key > pkes->at(mid).key && key < pkes->at(right).key) {
      left = mid;
      continue;
    }
  }
  if (left == right) {
    choose = left;
  }

  if (pkes->at(choose).key != key) {
    return type::ke_not_found;
  }

  Log("The choose key is %llu", pkes->at(right).key);
  return pkes->at(choose);
}

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
    kes.push_back(entry);
  }
  ifile.close();
}
} // namespace SSTable

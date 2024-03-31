#include "sstable.h"
#include "bloomfilter.h"
#include "type.h"
#include <cassert>
#include <fstream>

namespace SSTable {
u64 sstable_type::ss_total_uid = 1; // the first timestamp is 1
sstable_type::sstable_type(size_t BF_size, int hash_num)
    : ss_uid(ss_total_uid), bf_size(BF_size), hash_func_num(hash_num),
      BF(BF_size, hash_num), header({ss_total_uid, 0, 0, 0}) {
  // constructor implementation
  ss_total_uid++;
}
sstable_type::sstable_type(const kEntrys &kes, size_t BF_size, int hash_num)
    : ss_uid(ss_total_uid), bf_size(BF_size), hash_func_num(hash_num),
      BF(BF_size, hash_num), header({ss_total_uid, 0, 0, 0}), kes(kes) {
  // constructor implementation
  ss_total_uid++;
}
size_t sstable_type::size() {
  // num_of_kv: u64
  return header.getNumOfKV() * sizeof(u64) + sizeof(header) + bf_size / 8;
}

size_t sstable_type::cal_size(int number_of_kv, size_t BF_size) {
  return number_of_kv * sizeof(u64) + sizeof(header) + BF_size / 8;
}

void sstable_type::save(const std::string &path) {
  if (std::filesystem::exists(path)) {
    std::string msg = "Try to save to file(" + path + ") that already exists";
    throw(msg);
  }
  std::ofstream ofile(path, std::ios::binary | std::ios::trunc);
  // | Header | BF | kEntrys |
  ofile.write(reinterpret_cast<char *>(&header), sizeof(header));
  auto bytes = BF.toBytes();
  ofile.write(reinterpret_cast<char *>(bytes.data()), bytes.size());

  for (auto &entry : kes) {
    ofile.write(reinterpret_cast<char *>(&entry), sizeof(entry));
  }
  ofile.close();
}
void sstable_type::load(const std::string &path) {
  std::ifstream ifile(path, std::ios::binary);
  if (!std::filesystem::exists(path)) {
    std::string msg = "Try to load file(" + path + ") that not exists";
    throw(msg);
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

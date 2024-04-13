
#ifndef __SSTABLE_H
#define __SSTABLE_H
#include "bloomfilter.h"
#include "type.h"
#include <cstddef>
#include <memory>
namespace SSTable {

class Header {
private:
  u64 timeStamp;
  u64 num_of_kv;
  u64 minKey;
  u64 maxKey;

public:
  Header(u64 timeStamp_, u64 num_of_kv_, u64 minKey_, u64 maxKey_)
      : timeStamp(timeStamp_), num_of_kv(num_of_kv_), minKey(minKey_),
        maxKey(maxKey_) {}
  Header() : timeStamp(0), num_of_kv(0), minKey(0), maxKey(0) {}
  // getters and setters
  [[nodiscard]] u64 getTimeStamp() const { return timeStamp; }
  [[nodiscard]] u64 getNumOfKV() const { return num_of_kv; }
  [[nodiscard]] u64 getMinKey() const { return minKey; }
  [[nodiscard]] u64 getMaxKey() const { return maxKey; }
  void setTimeStamp(u64 ts) { timeStamp = ts; }
  void setNumOfKV(u64 num) { num_of_kv = num; }
  void setMinKey(u64 min) { minKey = min; }
  void setMaxKey(u64 max) { maxKey = max; }

  void clear() {
    timeStamp = 0;
    num_of_kv = 0;
    minKey = 0;
    maxKey = 0;
  }
};

class sstable_type {
  // NOTE: the sstable_type in mem act as cache for the sstable on disk
private:
  u64 bf_size;
  int hash_func_num;
  u64 ss_uid;
  static u64 ss_total_uid;
  static const u64 default_bf_size = 8 * 1024 * 8; // 8KB = 8*8*1024 bit
  BloomFilter BF;
  Header header;
  std::shared_ptr<kEntrys> pkes;
  u64 binary_search(TKey key, u64 total, bool &exist, bool use_BF = true) const;

public:
  static void resetID();
  sstable_type(u64 BF_size = default_bf_size, int hash_num = 3);
  sstable_type(const kEntrys &kes, u64 BF_size = default_bf_size,
               int hash_num = 3);
  sstable_type(const sstable_type &other);
  void addBF(const kEntrys &kes);
  u64 size() const;
  static u64 cal_size(int number_of_kv, u64 BF_size = default_bf_size);
  void save(const std::string &path);
  void load(const std::string &path);
  bool mayKeyExist(TKey key) const;
  void scan(TKey min, TKey max, kEntrys &res) const;
  kEntry query(TKey key) const;
  void clear();

  ~sstable_type() = default;
  u64 getUID() const { return ss_uid; }
  u64 getKEntryNum() const { return pkes->size(); }
};
} // namespace SSTable
#endif
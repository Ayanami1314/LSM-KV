
#ifndef __SSTABLE_H
#define __SSTABLE_H
#include "bloomfilter.h"
#include "type.h"
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
  Header(const Header &other)
      : timeStamp(other.timeStamp), num_of_kv(other.num_of_kv),
        minKey(other.minKey), maxKey(other.maxKey) {}
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
  u64 ss_uid;
  u64 bf_size;
  int hash_func_num;
  static u64 ss_total_uid;
  BloomFilter BF;
  Header header;
  std::shared_ptr<kEntrys> pkes;
  u64 binary_search(TKey key, u64 total, bool &exist, bool use_BF = true) const;

public:
  // static
  static void resetID();
  [[nodiscard]] static u64 getBFSize() { return config::bf_default_size; }
  [[nodiscard]] static std::string get_filename(const Header &h) {
    return std::to_string(h.getTimeStamp()) + "_" +
           std::to_string(h.getMinKey()) + "~" + std::to_string(h.getMaxKey()) +
           ".sst";
  }
  [[nodiscard]] static u64 incrTotalID() { return ++ss_total_uid; }

  // constructors
  sstable_type(u64 BF_size = config::bf_default_size,
               int hash_num = config::bf_default_k);
  sstable_type(const kEntrys &kes, u64 timeStamp,
               u64 BF_size = config::bf_default_size,
               int hash_num = config::bf_default_k);
  sstable_type(const sstable_type &other);
  sstable_type(kEntrys &&kes, u64 timeStamp, u64 BF_size, int hash_num);

  // methods
  void setID(u64 id) { this->ss_uid = id; }
  void addBF(const kEntrys &kes);
  [[nodiscard]] u64 size() const;
  [[nodiscard]] u64 size(std::string sstable_file) const;
  static u64 cal_size(int number_of_kv, u64 BF_size = config::bf_default_size);
  void save(const std::string &path);
  void load(const std::string &path);
  [[nodiscard]] bool mayKeyExist(TKey key) const;
  // [[nodiscard]] bool mayKeyExist(TKey key, std::string ss_file) const;
  void scan(TKey min, TKey max, kEntrys &res) const;
  [[nodiscard]] kEntry query(TKey key) const;
  // [[nodiscard]] kEntry query(TKey key, std::string ss_file) const;
  void clear();
  [[nodiscard]] BloomFilter getBF() const {
    if (!config::use_cache || !config::use_bf) [[unlikely]] {
      throw std::runtime_error("bf/cache is disabled");
    }
    return BF;
  }
  [[nodiscard]] std::shared_ptr<kEntrys> getKEntrys() const {
    if (!config::use_cache) [[unlikely]] {
      throw std::runtime_error("cache is disabled");
    }
    return this->pkes;
  }
  ~sstable_type() = default;
  [[nodiscard]] u64 getUID() const { return ss_uid; }
  [[nodiscard]] u64 getKEntryNum() const {
    if (!config::use_cache) [[unlikely]] {
      throw std::runtime_error("cache is disabled");
    }
    return pkes->size();
  }
  [[nodiscard]] Header getHeader() const {
    if (!config::use_cache) [[unlikely]] {
      throw std::runtime_error("cache is disabled");
    }
    return header;
  }

  [[nodiscard]] std::string gen_filename() const {
    if (!config::use_cache) [[unlikely]] {
      throw std::runtime_error("cache is disabled");
    }
    return get_filename(this->header);
  }
};
} // namespace SSTable
#endif
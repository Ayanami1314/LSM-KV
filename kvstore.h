#pragma once

#include "kvstore_api.h"
#include "skiplist.h"
#include "sstable.h"
#include "type.h"
#include "vlog.h"
#include <cmath>
#include <memory>
class KVStore : public KVStoreAPI {
  // You can add your implementation here
  using Layer = std::vector<SSTable::sstable_type>;
  using Layers = std::vector<Layer>;

private:
  std::unique_ptr<skiplist::skiplist_type> pkvs;
  const std::string save_dir;
  vLogs vStore;
  Layers ss_layers;

  static const size_t max_sz;

  void compaction();
  void save();
  size_t cal_new_size();
  size_t cal_new_size(size_t kv_num);
  inline int level_limit(int level) { return std::pow(2, level + 1); }
  void mergeLayers_Helper(int from,
                          const Layer &src); // overflow pass to the next layer
  void mergeLayers();

public:
  static const std::string delete_symbol;
  KVStore(const std::string &dir, const std::string &vlog);

  virtual ~KVStore();

  void put(uint64_t key, const std::string &s) override;

  std::string get(uint64_t key) override;

  bool del(uint64_t key) override;

  void reset() override;

  void scan(uint64_t key1, uint64_t key2,
            std::list<std::pair<uint64_t, std::string>> &list) override;

  void gc(uint64_t chunk_size) override;
  void convert_sst(SSTable::sstable_type &sst, vLogs &vl);
  // test-only
  void printMem();
  void clearMem();
  void rebuildMem();
};

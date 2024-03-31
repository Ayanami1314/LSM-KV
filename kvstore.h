#pragma once

#include "kvstore_api.h"
#include "skiplist.h"
#include "sstable.h"
#include <memory>
class KVStore : public KVStoreAPI {
  // You can add your implementation here
  using Layer = std::list<SSTable::sstable_type>;
  using Layers = std::list<std::tuple<Layer, int>>;

private:
  std::unique_ptr<skiplist::skiplist_type> pkvs;
  size_t sst_sz;
  size_t cal_new_size();
  static const std::string delete_symbol;
  static const size_t max_sz;
  const std::string save_dir;
  Layers ss_layers;
  void compaction();

public:
  KVStore(const std::string &dir);

  ~KVStore();

  void put(uint64_t key, const std::string &s) override;

  std::string get(uint64_t key) override;

  bool del(uint64_t key) override;

  void reset() override;

  void scan(uint64_t key1, uint64_t key2,
            std::list<std::pair<uint64_t, std::string>> &list) override;

  void gc(uint64_t chunk_size) override;
};

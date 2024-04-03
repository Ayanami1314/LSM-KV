#include "kvstore.h"
#include "skiplist.h"
#include "sstable.h"
#include "type.h"
#include "utils.h"
#include "vlog.h"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
const std::string KVStore::delete_symbol = "~DELETED~";
const size_t KVStore::max_sz = 16 * 1024; // 16KB
KVStore::KVStore(const std::string &dir)
    : KVStoreAPI(dir), save_dir(dir),
      pkvs(std::make_unique<skiplist::skiplist_type>()), sst_sz(0),
      vStore([dir]() {
        auto data_dir = std::filesystem::path(dir);
        if (!std::filesystem::exists(data_dir)) {
          utils::mkdir(data_dir);
        }
        utils::mkdir(std::filesystem::path(dir) / "level_0");
        // NOTE: vLog is a file
        return vLogs(std::filesystem::path(dir) / "vLog");
      }()) {}

KVStore::~KVStore() {
  // NOTE: now only level-0
  std::vector<std::string> save_paths;
  utils::scanDir(save_dir, save_paths);
  for (auto &p : save_paths) {
    utils::rmfile(p);
  }
  utils::rmdir(std::filesystem::path(save_dir) / "level_0");
}
size_t KVStore::cal_new_size() {
  return SSTable::sstable_type::cal_size(
      static_cast<int>(this->pkvs->size() + 1));
}
/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s) {
  // NOTE: check and update the sst_sz here
  size_t new_sz = cal_new_size();
  if (new_sz > max_sz) {
    // NOTE: write to sst
    // NOTE: reset sst_sz
    sst_sz = 0;
    // TODO: compaction();
  }
  pkvs->put(key, s);
  sst_sz = new_sz;
}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key) {
  // TODO: query in L0
  auto value = pkvs->get(key);
  return value == delete_symbol ? "" : value;
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key) {
  bool exist = pkvs->get(key) != delete_symbol;
  if (exist) {
    // bool exist_in_mem = !(pkvs->get(key) == "");
    // if (exist_in_mem) {
    // }
    pkvs->put(key, delete_symbol);
  }
  return exist;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset() {
  pkvs = std::make_unique<skiplist::skiplist_type>();
  sst_sz = 0;
}

/**
 * Return a list including all the key-value pair between key1 and key2.
 * keys in the list should be in an ascending order.
 * An empty string indicates not found.
 */
void KVStore::scan(uint64_t key1, uint64_t key2,
                   std::list<std::pair<uint64_t, std::string>> &list) {
  const auto first_scan = pkvs->scan(key1, key2);
  // filter the del element
  for (auto &kv : first_scan) {
    if (kv.second != delete_symbol) {
      list.push_back(kv);
    }
  }
}

/**
 * This reclaims space from vLog by moving valid value and discarding invalid
 * value. chunk_size is the size in byte you should AT LEAST recycle.
 */
void KVStore::gc(uint64_t chunk_size) {}

void KVStore::compaction() {
  // called when sst_sz > max_sz
  // TODO
}

void KVStore::convert_sst(SSTable::sstable_type &sst, vLogs &vl) {
  auto kvplist = pkvs->get_kvplist();
  kEntrys kes;
  for (auto kv : kvplist) {
    TOff offset = vl.addVlog({.key = kv.first,
                              .vlen = static_cast<TLen>(kv.second.size()),
                              .vvalue = kv.second},
                             false);

    kes.push_back({.key = kv.first,
                   .offset = offset,
                   .len = static_cast<TLen>(kv.second.size())});
  }
  sst = SSTable::sstable_type(kes);
}

void KVStore::save() {
  auto l0_dir = std::filesystem::path(save_dir) / "level_0";
  if (!std::filesystem::exists(l0_dir)) {
    throw("l0_dir does not exist");
  }
  if (!std::filesystem::is_directory(l0_dir)) {
    throw("l0_dir is not a directory");
  }
  SSTable::sstable_type new_sstable;
  convert_sst(new_sstable, vStore);
  std::string sst_filename = std::to_string(new_sstable.getUID()) + ".sst";
  auto sst_path = l0_dir / sst_filename;

  if (std::filesystem::exists(sst_path)) {
    throw("sstable already exists");
  }
  new_sstable.save(sst_path);
  vStore.sync();
}
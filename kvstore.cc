#include "kvstore.h"
#include "skiplist.h"
#include "sstable.h"
#include <memory>
#include <string>
const std::string KVStore::delete_symbol = "~DELETED~";
const size_t KVStore::max_sz = 16 * 1024; // 16KB
KVStore::KVStore(const std::string &dir)
    : KVStoreAPI(dir), pkvs(std::make_unique<skiplist::skiplist_type>()),
      sst_sz(0) {}

KVStore::~KVStore() {}
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
std::string KVStore::get(uint64_t key) { return pkvs->get(key); }
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key) {
  bool exist = !(pkvs->get(key) == "");
  if (exist) {
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
  list = pkvs->scan(key1, key2);
}

/**
 * This reclaims space from vLog by moving valid value and discarding invalid
 * value. chunk_size is the size in byte you should AT LEAST recycle.
 */
void KVStore::gc(uint64_t chunk_size) {}
#include "kvstore.h"
#include "skiplist.h"
#include "sstable.h"
#include "type.h"
#include "utils.h"
#include "vlog.h"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <queue>
#include <string>
const std::string KVStore::delete_symbol = "~DELETED~";
const size_t KVStore::max_sz = 16 * 1024; // 16KB
KVStore::KVStore(const std::string &dir, const std::string &vlog)
    : KVStoreAPI(dir, vlog), save_dir(dir),
      pkvs(std::make_unique<skiplist::skiplist_type>()), sst_sz(0),
      vStore([dir, vlog]() {
        if (!utils::dirExists(dir)) {
          utils::mkdir(dir);
        }
        return vlog;
      }()) {
  const std::string l0_dir = std::filesystem::path(save_dir) / "level_0";
  if (!utils::dirExists(l0_dir)) {
    utils::mkdir(l0_dir);
    Layer l0;
    ss_layers.push_back(l0);
  } else {
    rebuildMem();
  }
}

KVStore::~KVStore() {
  // // NOTE: now only level-0
  compaction();
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
    compaction();
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
  bool mem_exist = value != "";

  if (mem_exist) {
    return value == delete_symbol ? "" : value;
  }
  TValue v = "";
  // query in layers
  for (auto &layer : this->ss_layers) {
    // NOTE：the newest SST is the last one in the layer
    for (int i = layer.size() - 1; i >= 0; --i) {
      const auto &sst = layer[i];
      auto ke = sst.query(key); // HINT: header and BF: may exist?
      if (ke == type::ke_not_found) {
        continue;
      }
      if (ke == type::ke_deleted) {
        break;
      }
      if ((v = vStore.query(ke)) != "") {
        break;
      }
    }
  }
  return v;
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
  // clear file
  if (std::filesystem::exists(save_dir)) {
    std::vector<std::string> rmfiles;
    utils::scanDir(save_dir, rmfiles);
    for (auto &f : rmfiles) {
      auto path = std::filesystem::path(save_dir) / f;
      utils::rmfile(path);
    }
    // NOTE: not delete the dir itself
    // NOTE: create the .gitkeep file
    std::ofstream(save_dir + "/.gitkeep", std::ios::trunc);
  }
  // clear mem
  pkvs = std::make_unique<skiplist::skiplist_type>();
  vStore.clear();
  sst_sz = 0;

  // clear cache
  for (auto &layer : ss_layers) {
    for (auto &sst : layer) {
      sst.clear();
    }
  }
  // reset global state
  SSTable::sstable_type::resetID();
}

/**
 * Return a list including all the key-value pair between key1 and key2.
 * keys in the list should be in an ascending order.
 * An empty string indicates not found.
 */
void KVStore::scan(uint64_t key1, uint64_t key2,
                   std::list<std::pair<uint64_t, std::string>> &list) {
  kEntrys kvec;
  const auto mem_list = pkvs->scan(key1, key2);
  // filter the del element
  // for (auto &kv : mem_list) {
  //   if (kv.second != delete_symbol) {
  //     list.push_back(kv);
  //   }
  // }
  // scan in layers
  std::vector<kEntrys> layer_kvs;
  for (auto &layer : this->ss_layers) {
    for (int i = layer.size() - 1; i >= 0; --i) {
      auto &sst = layer[i];
      kEntrys tmp;
      sst.scan(key1, key2, tmp);
      if (tmp.size() != 0) {
        layer_kvs.push_back(tmp);
      }
      // HINT: now sorted by priority
    }
  }
  kEntrys merge_layers;
  utils::mergeKSorted(layer_kvs, merge_layers);

  // merge the memtable and the layers
  auto it = mem_list.begin();
  auto it2 = merge_layers.begin();
  while (it != mem_list.end() && it2 != merge_layers.end()) {
    if (it->first < it2->key) {
      if (it->second == delete_symbol) {
        // NOTE: attention this behavior
        // list.push_back(std::make_pair(it->first, ""));
        it++;
        continue;
      }
      it++;
      list.push_back(*it);
    } else if (it2->key < it->first) {
      if (it2->len == 0) {
        // list.push_back(std::make_pair(it->first, ""));
        it2++;
        continue;
      }

      list.push_back({it2->key, vStore.query(*it2)});
      it2++;
    } else {
      // key equal
      if (it->second == delete_symbol) {
        // NOTE: attention this behavior
        // list.push_back(std::make_pair(it->first, ""));
        it++;
        it2++;
        continue;
      }
      list.push_back(*it);
      it++;
      it2++;
    }
  }
  while (it != mem_list.end()) {
    if (it->second == delete_symbol) {
      // NOTE: attention this behavior
      // list.push_back(std::make_pair(it->first, ""));
      it++;
      continue;
    }
    list.push_back(*it);
    it++;
  }
  while (it2 != merge_layers.end()) {
    if (it2->len == 0) {
      // list.push_back(std::make_pair(it->first, ""));
      it2++;
      continue;
    }
    list.push_back({it2->key, vStore.query(*it2)});
    it2++;
  }
}

/**
 * This reclaims space from vLog by moving valid value and discarding
 * invalid value. chunk_size is the size in byte you should AT LEAST
 * recycle.
 */
void KVStore::gc(uint64_t chunk_size) {
  // TODO
  // HINT: the size of the value to be recycled is strictly no less than
  // chunk_size
  // NOTE: first, search the vlog tail
  vStore.relocTail();
  vEntrys ves;
  std::vector<TOff> locs;
  auto tail = vStore.getTail();
  size_t read_size = vStore.readVlogs(tail, ves, chunk_size, locs);

  // NOTE: second, find in lsmtree(search in SSTables)

  // HINT: ve -> key -> find the ke with the key in the sstables
  // -> new(loc of ve == ke.offset) ? insert to memtable : do nothing
  int idx = 0;
  for (auto &ve : ves) {

    kEntry ke = type::ke_not_found;
    bool found = false;
    bool mem = false;
    // try to found
    // in mem
    if (pkvs->get(ve.key) != "") {
      found = true;
      mem = true;
    }
    // in sstable
    if (!found) {
      for (auto &layer : this->ss_layers) {
        size_t layer_size = layer.size();
        // NOTE：the newest SST is the last one in the layer
        for (int i = layer_size - 1; i >= 0; --i) {
          const auto &sst = layer[i];
          ke = sst.query(ve.key); // HINT: header and BF: may exist?
          if (ke != type::ke_not_found) {
            // NOTE: because find order is from newest to oldest, so if found
            // and the offsets not equal, breaks also.
            found = true;
            break;
          }
        }
        if (found) {
          break;
        }
      }
    }

    if (!found) {
      ++idx;
      continue;
    }

    // NOTE: if in mem, do nothing
    // NOTE: if ** found ** and ** new ** and ** not delete **
    // insert to memtable
    // else do nothing
    else if (!mem && ke.offset == locs.at(idx) && ke.len != 0) {
      this->put(ke.key, ve.vvalue);
    }

    ++idx;
  }
  // NOTE: third, compaction and de_alloc_file
  compaction();
  utils::de_alloc_file(vStore.getPath(), tail, read_size);
  vStore.gc(tail + read_size);
}
/**
 * @brief compaction in sstable layers
 */
void KVStore::compaction() {
  // called when sst_sz > max_sz
  // TODO
  Log("compaction start:");
  save();
  auto l0_dir = std::filesystem::path(save_dir) / "level_0";
  if (!utils::dirExists(l0_dir)) {
    Log("l0_dir does not exist");
    return;
  }
  std::vector<std::string> l0_ssts;
  utils::scanDir(l0_dir, l0_ssts);
  if (l0_ssts.size() > level_limit(0)) {
    // TODO, merge
    Log("TODO: merge");
  }
}
/**
@brief convert the kvstore in mem to sstable and vlog. if deleted, the len of
the kEntry to be saved in sst will be 0
 * @param  sst
 * @param  vl
 */
void KVStore::convert_sst(SSTable::sstable_type &sst, vLogs &vl) {
  auto kvplist = pkvs->get_kvplist();
  kEntrys kes;
  for (auto kv : kvplist) {
    TLen len =
        kv.second == delete_symbol ? 0 : static_cast<TLen>(kv.second.size());
    TOff offset =
        vl.addVlog({.key = kv.first, .vlen = len, .vvalue = kv.second});
    kes.push_back({.key = kv.first, .offset = offset, .len = len});
  }
  sst = std::move(SSTable::sstable_type(kes));
}

/**
@brief save the sstable cache and the vlog cache to file
 */
void KVStore::save() {
  auto l0_dir = std::filesystem::path(save_dir) / "level_0";
  if (!std::filesystem::exists(l0_dir)) {
    Log("l0_dir %s does not exist", l0_dir);
    return;
  }
  if (!std::filesystem::is_directory(l0_dir)) {
    Log("l0_dir %s is not a directory", l0_dir);
    return;
  }
  SSTable::sstable_type new_sstable;
  convert_sst(new_sstable, vStore);
  // NOTE: update the cache

  std::string sst_filename = std::to_string(new_sstable.getUID()) + ".sst";

  auto sst_path = l0_dir / sst_filename;

  if (std::filesystem::exists(sst_path)) {
    Log("sstable %s already exists", sst_path);
    return;
  }
  new_sstable.save(sst_path);
  ss_layers[0].push_back(std::move(new_sstable));
  // clear the pkvs
  pkvs = std::make_unique<skiplist::skiplist_type>();
  sst_sz = 0;
}

/**
@brief simulate the emergence and test persistence
 */
void KVStore::clearMem() {
  compaction();
  pkvs = std::make_unique<skiplist::skiplist_type>();
  ss_layers = std::vector<Layer>();
  sst_sz = 0;
  vStore.clear_mem();
}

void KVStore::rebuildMem() {
  // NOTE: if the dir exists, load the sstables into cache
  std::vector<std::string> ssts;
  const std::string l0_dir = std::filesystem::path(save_dir) / "level_0";
  utils::scanDir(l0_dir, ssts);
  // ATTENTION: ssts is filenames! NOT paths!
  // NOTE FIX: restore the order by timeStamp
  std::sort(ssts.begin(), ssts.end(),
            [](const std::string &s1, const std::string &s2) {
              u64 timeStamp1 = std::stoull(s1.substr(0, s1.find('.')));
              u64 timeStamp2 = std::stoull(s1.substr(0, s1.find('.')));
              return timeStamp1 < timeStamp2;
            });
  Layer l0;
  for (auto &ss_name : ssts) {
    SSTable::sstable_type sst_cache;
    auto ss_path = std::filesystem::path(l0_dir) / ss_name;
    sst_cache.load(ss_path);
    l0.push_back(std::move(sst_cache));
  }
  // NOTE: set the vlog head and tail
  vStore.reload_mem();
  ss_layers.push_back(std::move(l0));
}
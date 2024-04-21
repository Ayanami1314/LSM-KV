#include "kvstore.h"
#include "skiplist.h"
#include "sstable.h"
#include "type.h"
#include "utils.h"
#include "vlog.h"
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <set>
#include <string>
#define KB (1024)
const std::string KVStore::delete_symbol = "~DELETED~";
const size_t KVStore::max_sz = 16 * KB; // 16KB
KVStore::KVStore(const std::string &dir, const std::string &vlog)
    : KVStoreAPI(dir, vlog), pkvs(std::make_unique<skiplist::skiplist_type>()),
      save_dir(dir), vStore([dir, vlog]() {
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
size_t KVStore::cal_new_size(size_t kv_num) {
  return SSTable::sstable_type::cal_size(kv_num);
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
    new_sz = 0;
    compaction();
  }
  this->pkvs->put(key, s);
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
  bool found = false;
  for (auto &layer : this->ss_layers) {
    // NOTE：the newest SST is the last one in the layer
    for (int i = layer.size() - 1; i >= 0; --i) {
      const auto &sst = layer[i];
      auto ke = sst.query(key); // HINT: header and BF: may exist?
      if (ke == type::ke_not_found) {
        continue;
      }
      if (ke.is_deleted()) {
        found = true;
        break;
      }
      if ((v = vStore.query(ke)) != "") {
        found = true;
        break;
      }
    }
    if (found) {
      break;
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
    std::filesystem::remove_all(save_dir);
    utils::mkdir(save_dir);
  }
  // clear mem
  pkvs = std::make_unique<skiplist::skiplist_type>();
  vStore.clear();
  // clear cache
  ss_layers = std::vector<Layer>();
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
    size_t layer_size = layer.size();
    for (int i = layer_size - 1; i >= 0; --i) {
      auto &sst = layer[i];
      kEntrys tmp;
      sst.scan(key1, key2, tmp);
      if (tmp.size() != 0) {
        layer_kvs.push_back(std::move(tmp));
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
  // TODO what if interrupt in gc?
  // HINT: the size of the value to be recycled is strictly no less than
  // chunk_size
  // NOTE: first, search the vlog tail
  vStore.relocTail();
  std::cout << "tail is " << vStore.getTail() << " now" << std::endl;
  vEntrys ves;
  std::vector<TOff> locs;
  auto tail = vStore.getTail();
  size_t read_size = vStore.readVlogs(tail, ves, chunk_size, locs);
  if (read_size == 0) {
    // vLog has no items now
    return;
  }
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
 * @brief
 * @param  layers
 * @param  from
 * @param  src  src should be independent(the copy of layers[i] or overflow)
 */
void KVStore::mergeLayers_Helper(int from, const Layer &src) {
  // ATTENTION: DO NOT USE PUSH/DEL TO CHANGE THE LAYERS IN THE FUNCTION!(ALWAYS
  // CONSIDER VECTOR REALLOC)
  // NOTE: we choose to update the cache first and then
  // the files first, choose the intersection part of src and dst NOTE: the sst
  // order in level >= 1 is guaranteed
  // TODO: use this to optimize merge(and completely change the
  // get/scan/cache/... use binary search in layer and break the "the newest one
  // in the last" rule)
  Layer new_overflow;
  int dst_level = from + 1;
  auto dst_save_dir =
      std::filesystem::path(save_dir) / ("level_" + std::to_string(dst_level));
  if (from + 1 > this->ss_layers.size() - 1) {
    ss_layers.push_back(Layer());
    if (!std::filesystem::exists(dst_save_dir)) {
      utils::mkdir(dst_save_dir);
    }
  }
  const Layer &dst = ss_layers.at(from + 1);

  const int limit = level_limit(dst_level);
  u64 merge_max_id = 0;
  for_each(src.begin(), src.end(),
           [&merge_max_id](const SSTable::sstable_type &sst) {
             merge_max_id =
                 merge_max_id > sst.getUID() ? merge_max_id : sst.getUID();
           });

  // from old to new
  std::vector<const SSTable::sstable_type *> intersection;
  std::vector<const SSTable::sstable_type *> no_intersection;
  std::set<std::string> no_save_files;
  // HINT: file in no-intersection and won't be saved again(but may be removed
  // because of overflow)
  std::vector<std::string> remove_files;
  // FIXME: intersection judge has bug
  for (const auto &d_sst : dst) {
    bool section_flag = false;
    for (const auto &s_sst : src) {
      TKey d_min = d_sst.getHeader().getMinKey();
      TKey d_max = d_sst.getHeader().getMaxKey();
      TKey s_min = s_sst.getHeader().getMinKey();
      TKey s_max = s_sst.getHeader().getMaxKey();
      // HINT: no intersection
      if (d_max < s_min || d_min > s_max) {
        continue;
      }
      // NOTE: iterate all pairs
      for (const auto &ke : *(d_sst.getKEntrys())) {
        if (s_sst.query(ke.key) != type::ke_not_found) {
          section_flag = true;
          break;
        }
      }

      if (section_flag) {
        remove_files.push_back(d_sst.gen_filename());
        // HINT: all intersection part files shoule be deleted
        intersection.push_back(&d_sst);

        section_flag = true;
        break;
      }
    }
    if (!section_flag) {
      no_intersection.push_back(&d_sst);
      no_save_files.insert(d_sst.gen_filename());
    }
  }
  // second, merge the intersection part
  std::vector<kEntrys> intersection_kes;
  kEntrys new_dst_kes;
  // HINT: priority: src from new to old, then dst from new to old to keep the
  // priority
  auto intersection_size = intersection.size();
  auto src_size = src.size();
  // change the order
  for (int i = src_size - 1; i >= 0; --i) {
    kEntrys kes;
    const SSTable::sstable_type &sst = src.at(i);
    sst.scan(sst.getHeader().getMinKey(), sst.getHeader().getMaxKey(), kes);
    intersection_kes.push_back(std::move(kes));
  }
  for (int i = intersection_size - 1; i >= 0; --i) {
    kEntrys kes;
    const SSTable::sstable_type *p_sst = intersection.at(i);
    p_sst->scan(p_sst->getHeader().getMinKey(), p_sst->getHeader().getMaxKey(),
                kes);
    intersection_kes.push_back(std::move(kes));
  }

  utils::mergeKSorted(intersection_kes, new_dst_kes);
  // generate new SSTables
  Layer merged_ssts;
  kEntrys tmp;
  int ke_num = 0;
  size_t no_intersection_size = no_intersection.size();
  size_t new_dst_kes_size = new_dst_kes.size();
  for (int i = 0; i < new_dst_kes_size; ++i) {
    tmp.push_back(new_dst_kes[i]);
    ++ke_num;
    if (cal_new_size(ke_num + 1) > max_sz) {
      ke_num = 0;
      SSTable::sstable_type new_sst(tmp, merge_max_id);
      merged_ssts.push_back(new_sst);
      tmp.clear();
      tmp.resize(0);
    }
  }
  if (tmp.size() != 0) {
    SSTable::sstable_type new_sst(tmp, merge_max_id);
    merged_ssts.push_back(std::move(new_sst));
    tmp.clear();
    tmp.resize(0);
  }
  // HINT: invert_tmp_stack: from new to old

  auto merged_ssts_size = merged_ssts.size();
  Layer final_dst;
  for (const auto *const sst : no_intersection) {
    final_dst.push_back(*sst);
  }
  for (auto &sst : merged_ssts) {
    final_dst.push_back(sst);
  }
  // HINT: newest in the begin
  std::sort(
      final_dst.begin(), final_dst.end(),
      [](const SSTable::sstable_type &s1, const SSTable::sstable_type &s2) {
        return s1.getUID() > s2.getUID() ||
               (s1.getUID() == s2.getUID() &&
                s1.getHeader().getMinKey() > s2.getHeader().getMinKey());
      });
  if (no_intersection_size + merged_ssts_size > limit) {
    // HINT: no_intersection is older and should be merged first
    int overflow_size = no_intersection_size + merged_ssts_size - limit;
    for (int i = 0; i < overflow_size; ++i) {
      auto &sst = final_dst.back();
      new_overflow.push_back(sst);
      remove_files.push_back(sst.gen_filename());
      final_dst.pop_back();
    }
  }
  assert(final_dst.size() <= limit);
  ss_layers.at(dst_level) = std::move(final_dst);

  // NOTE: sync to files
  if (!utils::dirExists(dst_save_dir)) {
    std::cerr << "dst_save_dir does not exist" << std::endl;
    assert(0);
    return;
  }
  std::vector<std::string> filenames;
  utils::scanDir(dst_save_dir, filenames);
  // NOTE: remove files to sync with cache
  for (auto &filename : remove_files) {
    auto sst_path = dst_save_dir / filename;
    if (!std::filesystem::exists(sst_path)) {
      continue;
    }
    utils::rmfile(sst_path);
  }
  // NOTE: add new merged file
  for (auto &sst : ss_layers.at(dst_level)) {
    // already exists
    if (no_save_files.find(sst.gen_filename()) != no_save_files.end()) {
      continue;
    }
    std::string sst_filename = sst.gen_filename();
    auto sst_path = dst_save_dir / sst_filename;
    sst.save(sst_path);
  }

  // NOTE: if the merged_ssts.size() + no_intersection.size() > limit,
  // recursion
  // TODO delete in last layer
  if (new_overflow.size() > 0) {
    mergeLayers_Helper(dst_level, new_overflow);
  }
}

/**
 * @brief compaction in sstable layers
 */
void KVStore::compaction() {
  // called when sst_sz > max_sz
  if (pkvs->size() == 0) {
    return;
  }
  Log("compaction start:");
  auto l0_dir = std::filesystem::path(save_dir) / "level_0";
  if (!utils::dirExists(l0_dir)) {
    utils::mkdir(l0_dir);
  }
  std::vector<std::string> l0_ssts;
  if (is_empty(l0_dir)) {
    save();
    // utils::printDir(save_dir);
    return;
  }
  utils::scanDir(l0_dir, l0_ssts);

  if (l0_ssts.size() + 1 > level_limit(0)) {
    // TODO, merge
    Log("merge start:");
    // HINT: merge all L0 and L1, update the cache
    // if L1 is full, merge the overflow part(choose the earlier and key is less
    // one) of L1 and L2, and so on
    // not save file in l0
    SSTable::sstable_type new_sstable;
    convert_sst(new_sstable, vStore);
    this->pkvs = std::make_unique<skiplist::skiplist_type>();
    ss_layers[0].push_back(new_sstable);
    // clear the pkvs
    // FIXME: push the level0 back, refactor the ss_layers
    Layer level_0_vec;
    for (auto &sst : ss_layers.at(0)) {
      level_0_vec.push_back(sst);
    }
    ss_layers.at(0) = Layer();
    mergeLayers_Helper(0, level_0_vec);
    // clear l0 file
    for (auto &sst : l0_ssts) {
      auto sst_path = l0_dir / sst;
      utils::rmfile(sst_path);
    }

  } else {
    save();
  }
  // utils::printDir(save_dir);
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
  auto timeStamp = SSTable::sstable_type::incrTotalID();
  sst = SSTable::sstable_type(kes, timeStamp);
}

/**
@brief save the sstable cache and the vlog cache to file
 */
void KVStore::save() {
  auto l0_dir = std::filesystem::path(save_dir) / "level_0";
  if (!std::filesystem::exists(l0_dir)) {
    Log("l0_dir %s does not exist", l0_dir.c_str());
    return;
  }
  if (!std::filesystem::is_directory(l0_dir)) {
    Log("l0_dir %s is not a directory", l0_dir.c_str());
    return;
  }
  SSTable::sstable_type new_sstable;
  convert_sst(new_sstable, vStore);
  // NOTE: update the cache

  std::string sst_filename = new_sstable.gen_filename();

  auto sst_path = l0_dir / sst_filename;

  if (std::filesystem::exists(sst_path)) {
    Log("sstable %s already exists", sst_path.c_str());
    assert(0);
  }
  new_sstable.save(sst_path);
  if (ss_layers.size() == 0) {
    ss_layers.push_back(Layer());
  }
  ss_layers[0].push_back(new_sstable);
  // clear the pkvs
  pkvs = std::make_unique<skiplist::skiplist_type>();
}

/**
@brief simulate the emergence and test persistence
 */
void KVStore::clearMem() {
  compaction();
  pkvs = std::make_unique<skiplist::skiplist_type>();
  ss_layers = std::vector<Layer>();
  vStore.clear_mem();
}

void KVStore::rebuildMem() {
  // NOTE: if the dir exists, load the sstables into cache
  int level = 0;
  std::string level_dir = std::filesystem::path(save_dir) / "level_0";
  while (utils::dirExists(level_dir)) {
    ++level;
    std::vector<std::string> ssts;
    utils::scanDir(level_dir, ssts);
    // ATTENTION: ssts is filenames! NOT paths!
    // NOTE FIX: restore the order by timeStamp
    std::sort(ssts.begin(), ssts.end(),
              [](const std::string &s1, const std::string &s2) {
                u64 timeStamp1 = std::stoull(s1.substr(0, s1.find('_')));
                u64 timeStamp2 = std::stoull(s2.substr(0, s2.find('_')));
                return timeStamp1 < timeStamp2;
              });
    Layer level_layer;
    for (auto &ss_name : ssts) {
      SSTable::sstable_type sst_cache;
      auto ss_path = std::filesystem::path(level_dir) / ss_name;
      sst_cache.load(ss_path);
      level_layer.push_back(sst_cache);
    }
    level_dir = (std::filesystem::path(save_dir) / "level_").string() +
                std::to_string(level);
    ss_layers.push_back(level_layer);
  }
  // NOTE: set the vlog head and tail
  if (std::filesystem::exists(vStore.getPath())) {
    vStore.reload_mem();
  }
}
void KVStore::printMem() { pkvs->print(); }
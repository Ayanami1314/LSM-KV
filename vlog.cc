#include "vlog.h"
#include "type.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <vector>
const u8 vLogs::magic = 0xff;
/**
 * @brief
 * @param  ifs
 * @param  ve
 * @return -1 if failed, 0 if success
 */
int read_a_ventry(std::ifstream &ifs, vEntry &ve) {
  // read bytes
  TMagic magic;
  TCheckSum checksum;
  TKey key;
  TLen vlen;
  TValue value;
  auto back_loc = ifs.tellg();

  ifs.read(reinterpret_cast<char *>(&magic), sizeof(magic));
  if (magic != vLogs::magic) {
    ifs.seekg(back_loc);
    return -1;
  }
  // ...
  ifs.read(reinterpret_cast<char *>(&checksum), sizeof(checksum));
  ifs.read(reinterpret_cast<char *>(&key), sizeof(key));
  ifs.read(reinterpret_cast<char *>(&vlen), sizeof(vlen));
  // HINT: an incorrect vlen can cause a crash here
  // SO USE next magic to act as end symbol
  // TBytes data(vlen);
  std::vector<u8> data;
  unsigned char byte;
  ifs.read(reinterpret_cast<char *>(&byte), sizeof(byte));
  while (byte != vLogs::magic && !ifs.eof() && !ifs.fail()) {
    data.push_back(byte);
    ifs.read(reinterpret_cast<char *>(&byte), sizeof(byte));
  }
  if (byte == vLogs::magic) {
    // HINT: set the ifs back 1
    ifs.seekg(-1, std::ios_base::cur);
  }
  if (data.size() != vlen) {
    ifs.seekg(back_loc);
    return -1;
  }
  TValue ret(data.begin(), data.end());
  ve.magic = magic;
  ve.checksum = checksum;
  ve.key = key;
  ve.vlen = vlen;
  ve.vvalue = ret;
  return 0;
}
vLogs::vLogs(TPath vpath) : vfilepath(vpath) {
  // HINT: magic number is used for find the head(because the head and the tail
  // won't be persistent), 0x7f
  // HINT: checksum is the crc16 value calculated by {key, vlen, vvalue}
  // HINT: saved key for gc
  // check if can open file
  reload_mem();
}
vLogs::~vLogs() {}
/**
@brief add a value entry to vlog (not change the vlog if the entry is marked
deleted)
 * @param  v
 * @param  sync if sync to vlog file immediately
 * @return TOff the write location in file
 */
TOff vLogs::addVlog(const vEntryProps &v) {
  // HINT: add a new vEntry to the ves
  // HINT: update the tail
  if (v.vlen == 0) {
    // deleted element
    return 0;
  }
  TCheckSum checksum;
  TBytes bytes = cal_bytes(v, checksum);
  std::ofstream ofs(vfilepath, std::ios::binary | std::ios::app);
  // std::printf("Checksum of v is: %x\n", checksum);
  ofs.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());

  // sync with file
  ofs.flush();
  ofs.close();
  u64 ret = head;
  head += bytes.size(); // head 在前面，gc从tail开始
  return ret;
}
/**
@brief the tail will be set to the begin the first magic
 */
void vLogs::relocTail() {
  u64 tmp = utils::seek_data_block(vfilepath);
  std::ifstream ifs(vfilepath, std::ios::binary);
  ifs.seekg(tmp);
  std::cout << "reloc to" << tmp << std::endl;
  unsigned char byte;
  vEntry ve;
  TCheckSum cal_checksum;
  auto back_loc = ifs.tellg();
  ifs.read(reinterpret_cast<char *>(&byte), sizeof(byte));
  while (byte != vLogs::magic && !ifs.eof() && !ifs.fail()) {
    ifs.read(reinterpret_cast<char *>(&byte), sizeof(byte));
  }
  if (ifs.eof() || ifs.fail()) {
    Log("relocTail: incorrect offset");
    ifs.seekg(back_loc);
    return;
  }
  ifs.seekg(-1, std::ios_base::cur);
  tail = ifs.tellg();
  return;
}
/**
@brief read the first vEntry from vlog
 * @param  offset
 * @param  ve
 */
void vLogs::readVlog(TOff offset, vEntry &ve) {
  relocTail();
  // Now the tail is set to the begin of the first valid entry
  std::ifstream ifs(vfilepath, std::ios::binary);
  if (!ifs.is_open()) {
    std::cerr << "In readVlog: Failed to open file" << std::endl;
    ve = type::ve_not_found;
  }
  ifs.seekg(tail);
  int suc = read_a_ventry(ifs, ve);
  if (suc == -1) {
    std::cerr << "readVlog: incorrect offset" << std::endl;
    ifs.close();
    return;
  }
  ifs.close();
}

/**
@brief read vEntrys util the read bytes reach the chunk_size
 * @param  offset
 * @param  ves should be empty
 * @param  chunk_size
 */
u64 vLogs::readVlogs(TOff offset, vEntrys &ves, u64 chunk_size,
                     std::vector<TOff> &locs) {
  relocTail();
  // Now the tail is set to the begin of the first valid entry
  std::ifstream ifs(vfilepath, std::ios::binary);
  if (!ifs.is_open()) {
    Log("In readVlog: Failed to open file");
    return 0;
  }
  vEntry ve;
  ifs.seekg(tail);
  locs.push_back(tail);
  auto begin = ifs.tellg();
  u64 size = 0;
  int loop_num = 0;
  while (size < chunk_size && size + begin < head) {
    loop_num++;
    if (loop_num % 10000 == 0) {
      std::cout << "readVlogs: endless loop? loop_num is: " << loop_num
                << std::endl;
    }
    int suc = read_a_ventry(ifs, ve);
    if (suc == -1) {
      Log("readVlogs: incorrect offset");
      ifs.close();
      return 0;
    }
    ves.push_back(ve);
    auto cur = ifs.tellg();
    size = cur - begin;
    locs.push_back(cur);
  }
  tail = size + begin;
  ifs.close();
  return size;
}
/**
@brief
 * @param  v vlog entry / entryProps
 * @param  checksum crc16
 * @return TBytes the bytes generated by vlog entry to write
 */
TBytes vLogs::cal_bytes(const vEntryProps &v, TCheckSum &checksum) {
  vEntry new_entry;
  TKey key = v.key;
  TLen vlen = v.vlen;
  TValue vvalue = v.vvalue;
  new_entry.key = key;
  new_entry.vlen = vlen;
  new_entry.vvalue = vvalue;

  TBytes bytes;
  bytes.push_back(magic);

  std::vector<u8> data;
  const u8 mask = 0xff;
  for (int i = 0; i < sizeof(key); ++i) {
    // little-endian
    u8 byte = (key >> (8 * i)) & mask;
    data.push_back(byte);
  }
  for (int i = 0; i < sizeof(vlen); ++i) {
    // little-endian
    u8 byte = (vlen >> (8 * i)) & mask;
    data.push_back(byte);
  }
  data.insert(data.end(), vvalue.begin(), vvalue.end());
  checksum = utils::crc16(data);

  for (int i = 0; i < sizeof(checksum); ++i) {
    // little-endian
    u8 byte = (checksum >> (8 * i)) & mask;
    bytes.push_back(byte);
  }

  bytes.insert(bytes.end(), data.begin(), data.end());
  return bytes;
}
/**
@brief
 * @param  v vlog entry / entryProps
 * @param  checksum crc16
 * @return TBytes the bytes generated by vlog entry to write
 */
TBytes vLogs::cal_bytes(const vEntry &v, TCheckSum &checksum) {
  vEntryProps vp = {v.key, v.vlen, v.vvalue};
  TBytes bytes = cal_bytes(vp, checksum);
  return bytes;
}
/**
@brief clear all(mem, disk, param)
 */
void vLogs::clear() {
  ves.clear();
  head = 0;
  tail = 0;
  std::ofstream ofs(vfilepath, std::ios::binary | std::ios::trunc);
  if (!ofs) {
    Log("In clear: Failed to open file");
    return;
  }
  ofs.close();
}
void vLogs::clear_mem() {
  ves.clear();
  head = 0;
  tail = 0;
}
void vLogs::reload_mem() {
  std::ifstream ifs(vfilepath, std::ios::binary | std::ios::ate); // move to end
  if (!ifs.is_open()) {
    Log("Failed to open file, vpath=%s", vfilepath);
    head = 0;
    tail = 0;
    return;
  }
  head = ifs.tellg();
  if (head == 0) {
    tail = 0;
    return;
  }
  this->relocTail();
  ifs.close();
}
/**
@brief
 * @param  ke key, off, len
 * @return TValue "" if fail or deleted. Otherwise, return the value
 */
TValue vLogs::query(kEntry ke) {
  std::ifstream ifs(vfilepath, std::ios::binary);
  if (!ifs.is_open()) {
    return "";
  }
  if (ke.offset > head || ke.offset < tail) {
    ifs.close();
    return "";
  }
  ifs.seekg(ke.offset);
  if (ifs.fail()) {
    ifs.clear();
    ifs.close();
    return "";
  }
  vEntry ve;
  int success = read_a_ventry(ifs, ve);
  ifs.close();
  if (success == -1)
    return "";
  return ve.vvalue;
}

void vLogs::gc(u64 new_tail) {
  if (new_tail <= tail) {
    Log("Error vlog gc: new_tail <= tail");
    return;
  }

  utils::de_alloc_file(vfilepath, tail, new_tail - tail);
  tail = new_tail;
}
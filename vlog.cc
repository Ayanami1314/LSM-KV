#include "vlog.h"
#include "type.h"
#include "utils.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
const u8 vLogs::magic = 0xff;
vLogs::vLogs(TPath vpath)
    : head(0), tail(0), vfilepath(vpath),
      ofs(vpath, std::ios::binary | std::ios::app) {
  // HINT: magic number is used for find the head(because the head and the tail
  // won't be persistent), 0x7f
  // HINT: checksum is the crc16 value calculated by {key, vlen, vvalue}
  // HINT: saved key for gc
  if (!ofs) {
    std::cout << "vpath: " << vpath << std::endl;
    throw std::runtime_error("Failed to open file");
  }
}
vLogs::~vLogs() {
  if (ofs.is_open()) {
    ofs.close();
  }
}
TOff vLogs::addVlog(const vEntryProps &v, bool sync) {
  // HINT: add a new vEntry to the ves
  // HINT: update the tail
  TCheckSum checksum;
  TBytes bytes = cal_bytes(v, checksum);
  std::printf("Checksum of v is: %x\n", checksum);
  ofs.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());

  // sync with file
  if (sync) {
    ofs.flush();
  }
  u64 ret = head;
  head += sizeof(vEntry); // head 在前面，gc从tail开始
  return ret;
}
void vLogs::sync() {
  if (!ofs.is_open()) {
    throw("In sync: the file should be open");
  }
  ofs.flush();
}
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
  std::cout << "vlog bytes size: " << bytes.size() << std::endl;
  return bytes;
}
TBytes vLogs::cal_bytes(const vEntry &v, TCheckSum &checksum) {
  vEntryProps vp = {v.key, v.vlen, v.vvalue};
  TBytes bytes = cal_bytes(vp, checksum);
  return bytes;
}

void vLogs::clear() {
  ves.clear();
  head = 0;
  tail = 0;
  if (ofs.is_open()) {
    ofs.close();
    ofs.open(vfilepath, std::ios::binary | std::ios::trunc);
    if (!ofs) {
      throw std::runtime_error("Failed to open file");
    }
  }
}
#ifndef __VLOG_H__
#define __VLOG_H__
#include "type.h"
#include <fstream>
class vLogs {
private:
  // TBytes data;

  u64 head;
  u64 tail;

  TPath vfilepath;
  std::ofstream ofs;

public:
  static const u8 magic;
  static TBytes cal_bytes(const vEntryProps &v, TCheckSum &checksum);
  static TBytes cal_bytes(const vEntry &v, TCheckSum &checksum);
  vEntrys ves;
  // vEntry:{char magic, uint16 checksum, uint32 vlen, uint64 value}

  // methods
  vLogs(TPath vpath);
  void relocTail();
  TOff addVlog(const vEntryProps &v,
               bool sync); // ret: the offset of the add vlog
  void append_file(TBytes data);
  void sync();
  void clear();
  [[nodiscard]] u64 getHead() const { return head; }
  [[nodiscard]] u64 getTail() const { return tail; }
  [[nodiscard]] u8 getMagic() const { return magic; }
  ~vLogs();
};
#endif
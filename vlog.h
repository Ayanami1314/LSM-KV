#ifndef __VLOG_H__
#define __VLOG_H__
#include "type.h"
class vLogs {
  private:
    // TBytes data;

    u64 head;
    u64 tail;

    TPath vfilepath;

  public:
    // TODO: add vlog cache
    static const u8 magic;
    static TBytes cal_bytes(const vEntryProps &v, TCheckSum &checksum);
    static TBytes cal_bytes(const vEntry &v, TCheckSum &checksum);
    vEntrys ves;
    // vEntry:{char magic, uint16 checksum, uint32 vlen, uint64 value}

    // methods
    vLogs(const TPath &vpath);
    void relocTail();
    TOff addVlog(const vEntryProps &v); // ret: the offset of the add vlog
    TValue query(kEntry ke);
    void clear();
    void clear_mem();
    void reload_mem();
    void gc(u64 new_tail);
    void readVlog(TOff offset, vEntry &ve);
    u64 readVlogs(TOff offset, vEntrys &ves, u64 chunk_size,
                  std::vector<TOff> &locs);
    [[nodiscard]] u64 getHead() const { return head; }
    [[nodiscard]] u64 getTail() const { return tail; }
    [[nodiscard]] u8 getMagic() const { return magic; }
    [[nodiscard]] std::string getPath() const { return vfilepath; }
    ~vLogs();
};
#endif
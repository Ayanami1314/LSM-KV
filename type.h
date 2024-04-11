#ifndef __TYPE_H
#define __TYPE_H
#include <cstdint>
#include <filesystem>
#include <list>
#include <string>
#include <vector>
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using TKey = u64;
using TOff = u64;
using TLen = u32;
using TCheckSum = u16;
using TMagic = u8;
using TValue = std::string;
using TBytes = std::vector<u8>;
using kEntry = struct kEntry {
  TKey key;
  TOff offset;
  TLen len;
  bool operator==(const kEntry &rhs) const {
    return key == rhs.key && offset == rhs.offset && len == rhs.len;
  }
  // NOTE：a < b. a's priority < b. a is deleted or a's key > b's or a's key ==
  // b's and a's offset < b's(older)
  bool operator<(const kEntry &rhs) const {
    return len == 0 || key > rhs.key || (key == rhs.key && offset < rhs.offset);
  }
  bool operator>(const kEntry &rhs) const {
    return rhs.len == 0 || key < rhs.key ||
           (key == rhs.key && offset > rhs.offset);
  }
};
using kEntrys = std::vector<kEntry>;
using vEntryPrefix = struct prefix {
  TMagic magic;
  TCheckSum checksum;
  TKey key;
  TLen vlen;
};

using vEntry = struct VEntry {
  TMagic magic;
  TCheckSum checksum;
  TKey key;
  TLen vlen;
  TValue vvalue;
};
using vEntryProps = struct VEntryProps {
  TKey key;
  TLen vlen;
  TValue vvalue;
};
using TPath = std::filesystem::path;
// vEntry:{char magic, uint16 checksum, uint32 vlen, string}

using vEntrys = std::list<vEntry>;
namespace type {
static kEntry ke_not_found = {0, 0, 0};
static vEntry ve_not_found = {0, 0, 0, 0, ""};
constexpr static size_t ve_prefix_len =
    sizeof(TMagic) + sizeof(TCheckSum) + sizeof(TKey) + sizeof(TLen);
} // namespace type
#endif
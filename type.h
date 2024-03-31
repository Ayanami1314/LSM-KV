#ifndef __TYPE_H
#define __TYPE_H
#include <cstdint>
#include <filesystem>
#include <list>
#include <string>
#include <tuple>
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
using kEntry = struct {
  TKey key;
  TOff offset;
  TLen len;
};
using kEntrys = std::list<kEntry>;
using vEntry = struct VEntrys {
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

#endif
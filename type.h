#ifndef __TYPE_H
#define __TYPE_H
#include <cstdint>
#include <list>
#include <tuple>
using u64 = uint64_t;
using u32 = uint32_t;
using TKey = u64;
using TOff = u64;
using TLen = u32;
using kEntry = std::tuple<TKey, TOff, TLen>;
using kEntrys = std::list<kEntry>;
#endif
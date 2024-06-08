#ifndef __CONFIG_H
#define __CONFIG_H
#include <iostream>

namespace config {
extern int bf_default_k;
extern uint64_t bf_default_size; // 8KB = 8*8*1024 bit
extern bool use_bf;
extern bool use_cache;
using ConfigParam = struct ConfigParam {
  int bf_default_size;
  int bf_default_k;
  bool use_bf;
  bool use_cache;
  friend std::ostream &operator<<(std::ostream &os, const ConfigParam &conf);
};
std::ostream &operator<<(std::ostream &os, const ConfigParam &conf);
void reConfig(const config::ConfigParam &newConfig);
} // namespace config
#endif
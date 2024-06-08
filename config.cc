#include "config.h"
#include "type.h"
namespace config {
int bf_default_k = 3;
u64 bf_default_size = 8 * 1024 * 8; // 8KB = 8*8*1024 bit
bool use_bf = true;
bool use_cache = true;
std::ostream &operator<<(std::ostream &os, const ConfigParam &conf) {
  auto out_format = "use_bf: %s, use_cache: %s\nbf_size: %d, bf_func_num: %d\n";
  char out[256];
  auto tf_tos = [](bool cond) { return cond ? "true" : "false"; };

  sprintf(out, out_format, tf_tos(conf.use_bf), tf_tos(conf.use_cache),
          conf.bf_default_size, conf.bf_default_k);
  os << out;
  return os;
}
void reConfig(const config::ConfigParam &newConfig) {
  auto [bf_default_size, bf_default_k, use_bf, use_cache] = newConfig;
  if (!use_cache) {
    config::use_cache = false;
    config::use_bf = false;
    config::bf_default_k = 0;
    config::bf_default_size = 0;
  };
  if (use_cache && !use_bf) {
    config::use_cache = true;
    config::use_bf = false;
    config::bf_default_k = 0;
    config::bf_default_size = 0;
  }
  if (use_cache && use_bf) {
    config::use_cache = true;
    config::use_bf = true;
    config::bf_default_k = bf_default_k;
    config::bf_default_size = bf_default_size;
  }
}
} // namespace config
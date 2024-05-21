#include "kvstore.h"
#include "type.h"
#include "utils.h"
#include <array>
#include <cassert>
#include <chrono>
#include <chrono> // Include the <chrono> header
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <utility>
using std::cout, std::endl, std::vector, std::array, std::string,
    config::ConfigParam;
#define MB (1024 * 1024)
#define KB (1024)
#define DEBUG 1
#if DEBUG
#define Assert(cond, msg, ...)                                                 \
  do {                                                                         \
    if (!cond) {                                                               \
      printf(msg, ##__VA_ARGS__);                                              \
      fflush(stdout);                                                          \
      assert(0);                                                               \
    }                                                                          \
  } while (0)
#else
#define Assert(cond, msg)
#endif
string random_str(int n) {
  string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  string new_str;
  for (int i = 0; i < n; ++i) {
    new_str.push_back(str[rand() % str.size()]);
  }
  return new_str;
}
std::mt19937 gen(42);
uint64_t random_key(int min, int max) {
  static std::uniform_int_distribution<> dis(min, max);
  return dis(gen);
}
void cal_time(std::function<void()> fn, string name, int repeat) {
  auto start = std::chrono::high_resolution_clock::now();
  fn();
  auto end = std::chrono::high_resolution_clock::now();
  auto duration_us =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  printf("%s: %lu us\n", name.c_str(), duration_us.count() / repeat);
  printf("\tthroughput: %f/s\n", repeat * 1e6 / duration_us.count());
}
void test(config::ConfigParam conf, int value_size, int prebuilt_data_num) {
  // prebuilt

  // ...

  if (utils::dirExists("./data")) {
    std::string cmd = "rm -rf ./data";
    system(cmd.c_str());
    std::this_thread::sleep_for(
        std::chrono::seconds(1)); // Use std::chrono::seconds for sleep_for
  }
  config::reConfig(conf);
  auto kvs = std::make_shared<KVStore>("./data", "./data/vlog");

  vector<std::pair<int, string>> test_kvs;
  for (int i = 0; i < prebuilt_data_num; ++i) {
    string value;
    do {
      value = random_str(value_size);
    } while (value == KVStore::delete_symbol);
    auto kvpair = std::make_pair(i, value);

    test_kvs.push_back(kvpair);
    kvs->put(kvpair.first, kvpair.second);
  }

  std::shuffle(test_kvs.begin(), test_kvs.end(), std::mt19937(17));

  cout << "value_size: " << value_size
       << ", prebuilt data num:" << prebuilt_data_num << endl;
  // test api
  // get 1000 times
  const int get_test_times = 10000;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, prebuilt_data_num - 1); // [a, b]
  auto get_fn = [&kvs, &test_kvs, prebuilt_data_num]() {
    for (int i = 0; i < get_test_times; ++i) {
      int choose = i % prebuilt_data_num;
      string value = kvs->get(test_kvs[choose].first);
      Assert((value == test_kvs[choose].second), "get error: get %s\n",
             value.c_str());
    }
  };
  cal_time(get_fn, "Get", get_test_times);
  // put 1000 times
  const int put_test_times = 1000;
  vector<string> random_strs(put_test_times, random_str(value_size));
  // 提前生成随机字符串
  auto put_fn = [&kvs, &test_kvs]() {
    for (int i = 0; i < put_test_times; ++i) {
      kvs->put(test_kvs[i].first, test_kvs[i].second);
    }
  };
  cal_time(put_fn, "Put", put_test_times);
  // del 1000 times
  const int del_test_times = 1000;
  auto del_fn = [&kvs, &test_kvs]() {
    for (int i = 0; i < del_test_times; ++i) {
      kvs->del(test_kvs[i].first);
    }
  };
  cal_time(del_fn, "Del", del_test_times);
  // scan 100 times
  const int scan_test_times = 100;
  auto scan_fn = [&kvs, prebuilt_data_num]() {
    for (int i = 0; i < scan_test_times; ++i) {
      std::list<std::pair<uint64_t, std::string>> list;
      auto start = random_key(0, prebuilt_data_num - 1);
      auto end = random_key(start, prebuilt_data_num - 1);
      kvs->scan(start, end, list);
    }
  };
  cal_time(scan_fn, "Scan", scan_test_times);
}
int main() {

  std::vector<std::pair<int, int>> test_params;
  test_params.emplace_back(10, 1000);
  test_params.emplace_back(100, 1000);
  test_params.emplace_back(1000, 1000);
  test_params.emplace_back(10000, 1000);
  test_params.emplace_back(10, 3000);
  test_params.emplace_back(100, 3000);
  test_params.emplace_back(1000, 3000);
  test_params.emplace_back(10000, 3000);
  test_params.emplace_back(10, 5000);
  test_params.emplace_back(100, 5000);
  test_params.emplace_back(1000, 5000);
  test_params.emplace_back(10000, 5000);

  // NOTE: 四种API测试, 1. put 2. get 3. del 4. scan
  // whith cache and bf
  const ConfigParam conf1 = {8 * 8 * 1024, 3, true, true};
  // HINT: default config
  cout << conf1;
  for (auto [value_size, num] : test_params) {
    test(conf1, value_size, num);
  }
  cout << endl << endl;
  cout << "Without BF" << endl;
  const ConfigParam conf2 = {0, 0, false, true};

  cout << conf2;
  for (auto [value_size, num] : test_params) {
    test(conf2, value_size, num);
  }
  cout << endl << endl;

  cout << "Without Cache and BF" << endl;
  const ConfigParam conf3 = {0, 0, false, false};
  cout << conf3;
  for (auto [value_size, num] : test_params) {
    test(conf3, value_size, num);
  }

  cout << endl << endl;
  cout << "Test with different BloomFilter size" << endl;
  for (int i = 1; i < 16; ++i) {
    int bf_size = i * 1024 * 8;
    cout << "BloomFilter size: " << i * 1024 << " Bytes" << endl;
    const ConfigParam conf_i = {bf_size, 3, true, true};
    for (auto [value_size, num] : test_params) {
      test(conf_i, value_size, num);
    }
  };
}
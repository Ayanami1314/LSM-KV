#include "../kvstore.h"
#include "../skiplist.h"
#include "../utils.h"
#include <fstream>
#include <gtest/gtest.h>
#include <random>
#include <string>
using std::string, std::vector, std::cout, std::endl, config::ConfigParam;
#define GC_EXPECT(cur, last, size)                                             \
  gc_expect<decltype(last)>((cur), (last), (size), __FILE__, __LINE__)
template <typename T>
void gc_expect(const T &cur, const T &last, const T &size,
               const std::string &file, int line) {
  if (cur >= last + size) {
    return;
  }
  std::cerr << "TEST Error @" << file << ":" << line;
  std::cerr << ", current offset " << cur;
  std::cerr << ", last offset " << last << std::endl;
}
class KVStoreTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).
    pStore = make_unique<KVStore>(testdir, vLog.string());
    pStore->reset();
    if (!utils::dirExists(testdir))
      utils::mkdir(testdir);
    // create file
    std::ofstream ofs(vLog, std::ios::binary);
    if (ofs.is_open()) {
      ofs.close();
    }
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
    // pStore->reset();
  }
  void check_gc(uint64_t size) {
    std::cout << "check_gc: " << size << std::endl;
    uint64_t last_offset, cur_offset;
    last_offset = utils::seek_data_block(vLog.c_str());
    pStore->gc(size);

    cur_offset = utils::seek_data_block(vLog.c_str());
    GC_EXPECT(cur_offset, last_offset, size);
    std::cout << "check_gc over" << std::endl;
  }
  std::filesystem::path testdir = "../tmp";
  std::filesystem::path vLog = testdir / "vlog";
  std::unique_ptr<KVStore> pStore;

  uint64_t random_key(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
  }
  void cal_time(std::function<void()> fn, std::string name, int repeat) {
    auto start = std::chrono::high_resolution_clock::now();
    fn();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    printf("%s: %lu us\n", name.c_str(), duration_us.count() / repeat);
    printf("\tthroughput: %f/s\n", repeat * 1e6 / duration_us.count());
  }
  std::string random_str(int n) {
    std::string str =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string new_str;
    for (int i = 0; i < n; ++i) {
      new_str.push_back(str[rand() % str.size()]);
    }
    return new_str;
  }
  void test(config::ConfigParam conf, int value_size, int prebuilt_data_num) {
    // prebuilt

    // ...

    if (utils::dirExists("./data")) {
      utils::rmDirRecursively("./data");
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
        ASSERT_TRUE(value == test_kvs[choose].second);
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
    auto scan_fn = [&kvs, prebuilt_data_num, this]() {
      for (int i = 0; i < scan_test_times; ++i) {
        std::list<std::pair<uint64_t, std::string>> list;
        auto start = random_key(0, prebuilt_data_num - 1);
        auto end = random_key(start, prebuilt_data_num - 1);
        kvs->scan(start, end, list);
      }
    };
    cal_time(scan_fn, "Scan", scan_test_times);
  }
};

TEST_F(KVStoreTest, basicCRUD) {
  EXPECT_EQ(pStore->get(1), "");
  EXPECT_EQ(pStore->get(2), "");

  pStore->put(1, "1");
  pStore->put(2, "2");
  pStore->put(5, "5");

  EXPECT_EQ(pStore->get(1), "1");
  EXPECT_EQ(pStore->get(5), "5");
  EXPECT_EQ(pStore->get(2), "2");
  EXPECT_EQ(pStore->get(3), "");

  pStore->del(1);
  EXPECT_EQ(pStore->get(1), "");
  pStore->put(6, "6");
  pStore->put(7, "7");
  pStore->put(8, "8");
  pStore->del(6);
  EXPECT_EQ(pStore->get(6), "");
  EXPECT_EQ(pStore->get(7), "7");
  EXPECT_EQ(pStore->get(5), "5");

  pStore->reset();
  EXPECT_EQ(pStore->get(8), "");
  EXPECT_EQ(pStore->get(7), "");
}

TEST_F(KVStoreTest, basicScan) {
  for (int i = 0; i < 10; ++i) {
    pStore->put(i, std::to_string(i));
  }
  auto ref = std::list<skiplist::kvpair>();
  auto ref2 = std::list<skiplist::kvpair>();
  for (int i = 1; i <= 3; ++i) {
    ref.push_back({i, std::to_string(i)});
  }
  for (int i = 5; i < 10; ++i) {
    ref2.push_back({i, std::to_string(i)});
  }
  auto res = std::list<skiplist::kvpair>();
  ;
  auto res2 = std::list<skiplist::kvpair>();
  ;
  pStore->scan(1, 3, res);
  pStore->scan(5, 9, res2);
  EXPECT_EQ(res, ref);
  EXPECT_EQ(res2, ref2);
}
TEST_F(KVStoreTest, BasicScan2) {
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;
  int max = 10;
  for (int i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
    pStore->put(i, std::to_string(i));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  EXPECT_EQ(list_ans.size(), list_stu.size());
}
TEST_F(KVStoreTest, BasicScan3) {
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;
  int max = 11;
  for (int i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
    pStore->put(i, std::to_string(i));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  EXPECT_EQ(list_ans, list_stu);
}
TEST_F(KVStoreTest, ScanWithDel) {
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;
  int max = 11;
  for (int i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
    pStore->put(i, std::to_string(i));
  }
  pStore->del(0);
  pStore->scan(0, max / 2 - 1, list_stu);
  EXPECT_EQ(list_ans.size(), list_stu.size() + 1);
  list_ans.pop_front();
  EXPECT_EQ(list_ans, list_stu);
}
TEST_F(KVStoreTest, BasicScan4) {
  pStore->put(1, "SE");
  pStore->del(1);
  auto res = std::list<skiplist::kvpair>();
  auto ref = std::list<skiplist::kvpair>();
  pStore->scan(1, 2, res);
  EXPECT_EQ(res, ref);
}
TEST_F(KVStoreTest, RegularTest50) {

  uint64_t i;
  const std::string not_found = "";
  const int max = 50;
  // Test a single key
  EXPECT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  EXPECT_EQ("SE", pStore->get(1));
  EXPECT_EQ(true, pStore->del(1));
  EXPECT_EQ(not_found, pStore->get(1));
  EXPECT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    EXPECT_EQ(std::to_string(i), pStore->get(i));
  }

  // Test after all insertions
  for (i = 0; i < max; ++i)
    EXPECT_EQ(std::to_string(i), pStore->get(i));

  // Test scan
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;

  for (i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  EXPECT_EQ(list_ans.size(), list_stu.size());

  auto ap = list_ans.begin();
  auto sp = list_stu.begin();
  while (ap != list_ans.end()) {
    if (sp == list_stu.end()) {
      EXPECT_EQ((*ap).first, -1);
      EXPECT_EQ((*ap).second, not_found);
      ap++;
    } else {
      EXPECT_EQ((*ap).first, (*sp).first);
      EXPECT_EQ((*ap).second, (*sp).second);
      ap++;
      sp++;
    }
  }

  // Test deletions
  for (i = 0; i < max; i += 2) {
    EXPECT_EQ(true, pStore->del(i));
  }

  for (i = 0; i < max; ++i)
    EXPECT_EQ((i & 1) ? std::to_string(i) : not_found, pStore->get(i));

  for (i = 1; i < max; ++i)
    EXPECT_EQ(i & 1, pStore->del(i));
}

TEST_F(KVStoreTest, RegularTest200) {
  uint64_t i;
  const std::string not_found = "";
  const int max = 200;
  // Test a single key
  EXPECT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  EXPECT_EQ("SE", pStore->get(1));
  EXPECT_EQ(true, pStore->del(1));
  EXPECT_EQ(not_found, pStore->get(1));
  EXPECT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    EXPECT_EQ(std::to_string(i), pStore->get(i));
  }

  // Test after all insertions
  for (i = 0; i < max; ++i)
    EXPECT_EQ(std::to_string(i), pStore->get(i));

  // Test scan
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;

  for (i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  EXPECT_EQ(list_ans.size(), list_stu.size());

  auto ap = list_ans.begin();
  auto sp = list_stu.begin();
  while (ap != list_ans.end()) {
    if (sp == list_stu.end()) {
      EXPECT_EQ((*ap).first, -1);
      EXPECT_EQ((*ap).second, not_found);
      ap++;
    } else {
      EXPECT_EQ((*ap).first, (*sp).first);
      EXPECT_EQ((*ap).second, (*sp).second);
      ap++;
      sp++;
    }
  }

  // Test deletions
  for (i = 0; i < max; i += 2) {
    EXPECT_EQ(true, pStore->del(i));
  }

  for (i = 0; i < max; ++i)
    EXPECT_EQ((i & 1) ? std::to_string(i) : not_found, pStore->get(i));

  for (i = 1; i < max; ++i)
    EXPECT_EQ(i & 1, pStore->del(i));
}

TEST_F(KVStoreTest, RegularTest1024) {
  uint64_t i;
  const std::string not_found = "";
  const int max = 1024;
  // Test a single key
  EXPECT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  EXPECT_EQ("SE", pStore->get(1));
  EXPECT_EQ(true, pStore->del(1));
  EXPECT_EQ(not_found, pStore->get(1));
  EXPECT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    EXPECT_EQ(std::to_string(i), pStore->get(i));
  }

  // Test after all insertions
  for (i = 0; i < max; ++i)
    EXPECT_EQ(std::to_string(i), pStore->get(i));

  // Test scan
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;

  for (i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  EXPECT_EQ(list_ans.size(), list_stu.size());

  auto ap = list_ans.begin();
  auto sp = list_stu.begin();
  while (ap != list_ans.end()) {
    if (sp == list_stu.end()) {
      EXPECT_EQ((*ap).first, -1);
      EXPECT_EQ((*ap).second, not_found);
      ap++;
    } else {
      EXPECT_EQ((*ap).first, (*sp).first);
      EXPECT_EQ((*ap).second, (*sp).second);
      ap++;
      sp++;
    }
  }

  // Test deletions
  for (i = 0; i < max; i += 2) {
    EXPECT_EQ(true, pStore->del(i));
  }

  for (i = 0; i < max; ++i)
    EXPECT_EQ((i & 1) ? std::to_string(i) : not_found, pStore->get(i));

  for (i = 1; i < max; ++i)
    EXPECT_EQ(i & 1, pStore->del(i));
}

TEST_F(KVStoreTest, JustOverflow) {
  uint64_t i;
  const std::string not_found = "";
  const int max = 350;
  // Test a single key
  EXPECT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  EXPECT_EQ("SE", pStore->get(1));
  EXPECT_EQ(true, pStore->del(1));
  EXPECT_EQ(not_found, pStore->get(1));
  EXPECT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    EXPECT_EQ(std::to_string(i), pStore->get(i));
  }

  // // Test after all insertions
  for (i = 0; i < max; ++i)
    EXPECT_EQ(std::to_string(i), pStore->get(i));
}
TEST_F(KVStoreTest, ThreeSST) {
  uint64_t i;
  const std::string not_found = "";
  const int max = 1000;
  // Test a single key
  EXPECT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  EXPECT_EQ("SE", pStore->get(1));
  EXPECT_EQ(true, pStore->del(1));
  EXPECT_EQ(not_found, pStore->get(1));
  EXPECT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    EXPECT_EQ(std::to_string(i), pStore->get(i));
  }

  // // Test after all insertions
  for (i = 0; i < max; ++i)
    EXPECT_EQ(std::to_string(i), pStore->get(i));
}

TEST_F(KVStoreTest, LargeScan) {
  for (int i = 0; i < 1000; ++i) {
    pStore->put(i, std::to_string(i));
  }
  auto ref = std::list<skiplist::kvpair>();
  auto res = std::list<skiplist::kvpair>();
  pStore->scan(100, 100000, res);

  for (int i = 100; i < 1000; ++i) {
    ref.push_back({i, std::to_string(i)});
  }

  EXPECT_EQ(ref, res);
}

TEST_F(KVStoreTest, LargeScanWithDel) {
  for (int i = 0; i < 1000; ++i) {
    pStore->put(i, std::to_string(i));
  }
  for (int i = 0; i < 1000; i += 2) {
    pStore->del(i);
  }
  auto ref = std::list<skiplist::kvpair>();
  auto res = std::list<skiplist::kvpair>();
  pStore->scan(100, 900, res);

  for (int i = 101; i <= 900; i += 2) {
    ref.push_back({i, std::to_string(i)});
  }

  EXPECT_EQ(ref, res);
}

TEST_F(KVStoreTest, AnotherLargeScan) {
  // Test after all insertions
  uint64_t i;
  auto not_found = "";
  int max = 500;
  // Test a single key
  EXPECT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  EXPECT_EQ("SE", pStore->get(1));
  EXPECT_EQ(true, pStore->del(1));
  EXPECT_EQ(not_found, pStore->get(1));
  EXPECT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    // EXPECT_EQ(std::to_string(i), pStore->get(i));
  }

  // for (i = 0; i < max; ++i)
  //   EXPECT_EQ(std::to_string(i), pStore->get(i));

  // Test scan
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;

  for (i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
  }

  pStore->scan(0, max / 2 - 1, list_stu);

  EXPECT_EQ(list_ans.size(), list_stu.size());

  // auto ap = list_ans.begin();
  // auto sp = list_stu.begin();
  // while (ap != list_ans.end()) {
  //   if (sp == list_stu.end()) {
  //     EXPECT_EQ((*ap).first, -1);
  //     EXPECT_EQ((*ap).second, not_found);
  //     ap++;
  //   } else {
  //     EXPECT_EQ((*ap).first, (*sp).first);
  //     EXPECT_EQ((*ap).second, (*sp).second);
  //     ap++;
  //     sp++;
  //   }
  // }
}
TEST_F(KVStoreTest, AnotherLargeScanWithDel) {
  uint64_t i;
  auto not_found = "";
  int max = 345;
  // Test a single key
  EXPECT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  EXPECT_EQ("SE", pStore->get(1));
  EXPECT_EQ(true, pStore->del(1));
  EXPECT_EQ(not_found, pStore->get(1));
  EXPECT_EQ(false, pStore->del(1));
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
  }

  // Test deletions
  for (i = 0; i < max; i += 2) {
    EXPECT_EQ(true, pStore->del(i));
  }

  for (i = 0; i < max; ++i)
    EXPECT_EQ((i & 1) ? std::to_string(i) : not_found, pStore->get(i));

  for (i = 1; i < max; ++i)
    EXPECT_EQ(i & 1, pStore->del(i));
}

TEST_F(KVStoreTest, basicGC) {
#define KB (1024)

  int max = 1024;
  uint64_t i;
  uint64_t gc_trigger = 1024;

  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
  }
  std::cout << "Put end." << std::endl;
  pStore->printMem();
  for (i = 0; i < max; ++i) {
    // std::cout << "idx: " << i << std::endl;
    EXPECT_EQ(std::to_string(i), pStore->get(i));
    // std::cout << "after get" << std::endl;
    switch (i % 3) {
    case 0:
      pStore->put(i, std::to_string(i));
      break;
    case 1:
      pStore->put(i, std::to_string(i));
      break;
    case 2:
      pStore->put(i, std::to_string(i));
      break;
    default:
      assert(0);
    }

    if (i % gc_trigger == 0) [[unlikely]] {
      std::cout << "gc_start: " << i << std::endl;
      check_gc(16 * KB);
    }
  }
  std::cout << "Stage 1 end." << std::endl;
}

TEST_F(KVStoreTest, SmallPutOverride) {

  uint64_t i;
  int max = 1024;
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i) + 's');
  }
  std::cout << "Put end." << std::endl;
  for (i = 0; i < max; ++i) {
    EXPECT_EQ(std::string(std::to_string(i) + 's'), pStore->get(i));
    switch (i % 3) {
    case 0:
      pStore->put(i, std::to_string(i) + 'a');
      break;
    case 1:
      pStore->put(i, std::to_string(i) + 'b');
      break;
    case 2:
      pStore->put(i, std::to_string(i) + 'c');
      break;
    default:
      assert(0);
    }
  }
  std::cout << "Stage 1 end." << std::endl;
  pStore->printMem();
  for (i = 0; i < max; ++i) {
    switch (i % 3) {
    case 0:
      EXPECT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      EXPECT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      EXPECT_EQ(std::to_string(i) + 'c', pStore->get(i));
      break;
    default:
      assert(0);
    }
  }
}
TEST_F(KVStoreTest, MidPutOverride1) {
  uint64_t i;
  uint64_t gc_trigger = 1024;
  int max = 1024 * 2;
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i) + 's');
  }
  std::cout << "Put end." << std::endl;
  for (i = 0; i < max; ++i) {
    EXPECT_EQ(std::string(std::to_string(i) + 's'), pStore->get(i));
    switch (i % 3) {
    case 0:
      pStore->put(i, std::to_string(i) + 'a');
      EXPECT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      pStore->put(i, std::to_string(i) + 'b');
      EXPECT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      pStore->put(i, std::to_string(i) + 'c');
      EXPECT_EQ(std::to_string(i) + 'c', pStore->get(i));
      break;
    default:
      assert(0);
    }
    if (i % gc_trigger == 0) [[unlikely]] {
      check_gc(16 * KB);
    }
  }
  std::cout << "Stage 1 end." << std::endl;

  for (i = 0; i < max; ++i) {
    switch (i % 3) {
    case 0:
      EXPECT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      EXPECT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      EXPECT_EQ(std::to_string(i) + 'c', pStore->get(i));
      break;
    default:
      assert(0);
    }
  }
}

TEST_F(KVStoreTest, MidPutOverride2) {
  uint64_t i;
  uint64_t gc_trigger = 1024;
  int max = 1024 * 3;
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i) + 's');
  }
  std::cout << "Put end." << std::endl;
  for (i = 0; i < max; ++i) {
    EXPECT_EQ(std::string(std::to_string(i) + 's'), pStore->get(i));
    switch (i % 3) {
    case 0:
      pStore->put(i, std::to_string(i) + 'a');
      EXPECT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      pStore->put(i, std::to_string(i) + 'b');
      EXPECT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      pStore->put(i, std::to_string(i) + 'c');
      EXPECT_EQ(std::to_string(i) + 'c', pStore->get(i));
      break;
    default:
      assert(0);
    }
    if (i % gc_trigger == 0) [[unlikely]] {
      check_gc(16 * KB);
    }
  }
  std::cout << "Stage 1 end." << std::endl;

  for (i = 0; i < max; ++i) {
    switch (i % 3) {
    case 0:
      EXPECT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      EXPECT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      EXPECT_EQ(std::to_string(i) + 'c', pStore->get(i));
      break;
    default:
      assert(0);
    }
  }
}

TEST_F(KVStoreTest, LargePutOverride) {
  uint64_t i;
  uint64_t gc_trigger = 1024;
  int max = 1024 * 48;
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i) + 's');
  }
  std::cout << "Put end." << std::endl;
  for (i = 0; i < max; ++i) {
    EXPECT_EQ(std::string(std::to_string(i) + 's'), pStore->get(i));
    switch (i % 3) {
    case 0:
      pStore->put(i, std::to_string(i) + 'a');
      EXPECT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      pStore->put(i, std::to_string(i) + 'b');
      EXPECT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      pStore->put(i, std::to_string(i) + 'c');
      EXPECT_EQ(std::to_string(i) + 'c', pStore->get(i));
      break;
    default:
      assert(0);
    }
    if (i % gc_trigger == 0) [[unlikely]] {
      check_gc(16 * KB);
    }
  }
  std::cout << "Stage 1 end." << std::endl;

  for (i = 0; i < max; ++i) {
    switch (i % 3) {
    case 0:
      EXPECT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      EXPECT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      EXPECT_EQ(std::to_string(i) + 'c', pStore->get(i));
      break;
    default:
      assert(0);
    }
  }
}

TEST_F(KVStoreTest, LargeGC) {
  uint64_t i;
  uint64_t gc_trigger = 1024;
  std::string not_found = "";
  int max = 1024 * 48;
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i) + 's');
  }
  std::cout << "Put end." << std::endl;
  for (i = 0; i < max; ++i) {
    EXPECT_EQ(std::string(std::to_string(i) + 's'), pStore->get(i));
    switch (i % 3) {
    case 0:
      pStore->put(i, std::to_string(i) + 'a');
      EXPECT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      pStore->put(i, std::to_string(i) + 'b');
      EXPECT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      pStore->put(i, std::to_string(i) + 'c');

      EXPECT_EQ(std::to_string(i) + 'c', pStore->get(i));
      break;
    default:
      assert(0);
    }
    if (i % gc_trigger == 0) [[unlikely]] {
      check_gc(16 * KB);
    }
  }
  std::cout << "Stage 1 end." << std::endl;
  for (i = 0; i < max; i += 2) {
    pStore->del(i);
  }
  for (i = 0; i < max; ++i) {
    switch (i % 3) {
    case 0:
      EXPECT_EQ(i % 2 == 0 ? not_found : std::to_string(i) + 'a',
                pStore->get(i));
      break;
    case 1:
      EXPECT_EQ(i % 2 == 0 ? not_found : std::to_string(i) + 'b',
                pStore->get(i));
      break;
    case 2:
      EXPECT_EQ(i % 2 == 0 ? not_found : std::to_string(i) + 'c',
                pStore->get(i));
      break;
    default:
      assert(0);
    }
  }
}
TEST_F(KVStoreTest, largePutAndDel) {
  int max = 1000;
  for (int i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
  }

  for (int i = 0; i < max; i += 2) {
    pStore->del(i);
  }

  for (int i = 0; i < max; ++i) {
    EXPECT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}

TEST_F(KVStoreTest, MultiLayers) {
  int max = 10000;
  for (int i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
  }

  for (int i = 0; i < max; i += 2) {
    pStore->del(i);
  }

  for (int i = 0; i < max; ++i) {
    ASSERT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}
TEST_F(KVStoreTest, simulatedPersistence) {
  int max = 1000;
  for (int i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
  }

  for (int i = 0; i < max; i += 2) {
    pStore->del(i);
  }

  pStore->clearMem();
  pStore->rebuildMem();

  for (int i = 0; i < max; ++i) {
    EXPECT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}
TEST_F(KVStoreTest, smallPersisWithGC) {
  int max = 10;
  for (int i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
  }
  check_gc(60);
  for (int i = 0; i < max; i += 2) {
    pStore->del(i);
  }
  check_gc(60);
  pStore->clearMem();
  pStore->rebuildMem();

  for (int i = 0; i < max; ++i) {
    EXPECT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}
TEST_F(KVStoreTest, largerPersisWithGC) {
  int max = 350;
  for (int i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
  }
  check_gc(1 * KB);
  for (int i = 0; i < max; i += 2) {
    pStore->del(i);
  }
  check_gc(1 * KB);
  pStore->clearMem();
  pStore->rebuildMem();

  for (int i = 0; i < max; ++i) {
    EXPECT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}
TEST_F(KVStoreTest, simulatedPersistenceWithGC) {
  int max = 1024;
  for (int i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
  }
  check_gc(1 * KB);
  for (int i = 0; i < max; i += 2) {
    pStore->del(i);
  }
  check_gc(1 * KB);
  pStore->clearMem();
  pStore->rebuildMem();

  for (int i = 0; i < max; ++i) {
    EXPECT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}

TEST_F(KVStoreTest, randomLargeSizeSmallNumTest) {
  int value_size = 10000;
  int has_data_size = 10000 * 3000;
  // prebuilt
  if (utils::dirExists("./data")) {
    std::string cmd = "rm -rf ./data";
    system(cmd.c_str());
  }
  auto kvs = std::make_shared<KVStore>("./data", "./data/vlog");
  int prebuilt_data_num = has_data_size / value_size;

  vector<std::pair<int, std::string>> test_kvs;
  for (int i = 0; i < prebuilt_data_num; ++i) {
    auto kvpair = std::make_pair(i, random_str(value_size));
    test_kvs.push_back(kvpair);
    kvs->put(kvpair.first, kvpair.second);
  }

  std::shuffle(test_kvs.begin(), test_kvs.end(),
               std::mt19937(std::random_device()()));

  cout << "value_size: " << value_size << " has_data_size: " << has_data_size
       << endl;
  cout << "prebuilt_data_num: " << prebuilt_data_num << endl;
  // test api
  // get 1000 times
  const int get_test_times = 1000;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, prebuilt_data_num - 1); // [a, b]
  auto get_fn = [&kvs, &test_kvs]() {
    for (int i = 0; i < get_test_times; ++i) {
      string value = kvs->get(test_kvs[i].first);
      EXPECT_EQ(value, test_kvs[i].second);
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
  auto scan_fn = [this, &kvs, prebuilt_data_num]() {
    for (int i = 0; i < scan_test_times; ++i) {
      std::list<std::pair<uint64_t, std::string>> list;
      auto start = random_key(0, prebuilt_data_num - 1);
      auto end = random_key(start, prebuilt_data_num - 1);
      if (start > end) {
        auto tmp = start;
        start = end;
        end = tmp;
      }
      kvs->scan(start, end, list);
    }
  };
  cal_time(scan_fn, "Scan", scan_test_times);
}

TEST_F(KVStoreTest, randomSmallSizeSmallNumTest) {
  int value_size = 100;
  int has_data_size = 160 * 1024;
  // prebuilt
  if (utils::dirExists("./data")) {
    std::string cmd = "rm -rf ./data";
    system(cmd.c_str());
  }
  auto kvs = std::make_shared<KVStore>("./data", "./data/vlog");
  int prebuilt_data_num = has_data_size / value_size;

  vector<std::pair<int, std::string>> test_kvs;
  const int max_times = 1000;
  ASSERT_TRUE(prebuilt_data_num >= max_times);
  for (int i = 0; i < prebuilt_data_num; ++i) {
    auto kvpair = std::make_pair(i, random_str(value_size));
    test_kvs.push_back(kvpair);
    kvs->put(kvpair.first, kvpair.second);
  }

  std::shuffle(test_kvs.begin(), test_kvs.end(),
               std::mt19937(std::random_device()()));

  cout << "value_size: " << value_size << " has_data_size: " << has_data_size
       << endl;
  cout << "prebuilt_data_num: " << prebuilt_data_num << endl;
  // test api
  // get 1000 times
  const int get_test_times = 1000;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, prebuilt_data_num - 1); // [a, b]
  auto get_fn = [&kvs, &test_kvs]() {
    for (int i = 0; i < get_test_times; ++i) {
      string value = kvs->get(test_kvs[i].first);
      EXPECT_EQ(value, test_kvs[i].second);
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
  auto scan_fn = [this, &kvs, prebuilt_data_num]() {
    for (int i = 0; i < scan_test_times; ++i) {
      std::list<std::pair<uint64_t, std::string>> list;
      auto start = random_key(0, prebuilt_data_num - 1);
      auto end = random_key(start, prebuilt_data_num - 1);
      if (start > end) {
        auto tmp = start;
        start = end;
        end = tmp;
      }
      kvs->scan(start, end, list);
    }
  };
  cal_time(scan_fn, "Scan", scan_test_times);
}

TEST_F(KVStoreTest, fullRandomTest) {

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

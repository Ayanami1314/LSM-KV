#include "../kvstore.h"
#include "../skiplist.h"
#include "../utils.h"
#include <gtest/gtest.h>
#include <string>
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

    if (!utils::dirExists(testdir))
      utils::mkdir(testdir);
    pStore = make_unique<KVStore>(testdir, vLog);
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
    if (utils::dirExists(testdir)) {
      system(("rm -rf " + testdir).c_str());
    }
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
  std::string testdir = "../tmp";
  std::string vLog = "../tmp/vlog";
  std::unique_ptr<KVStore> pStore;
};

TEST_F(KVStoreTest, basicCRUD) {
  pStore->reset();
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
  for (i = 0; i < max; ++i) {
    // std::cout << "idx: " << i << std::endl;
    EXPECT_EQ(std::to_string(i), pStore->get(i));
    std::cout << "after get" << std::endl;
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
      // check_gc(16 * KB);
    }
  }
  std::cout << "Stage 1 end." << std::endl;
}

TEST_F(KVStoreTest, SmallPutOverride) {
  pStore->reset();
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
    std::cout << "idx: " << i << std::endl;
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
  pStore->reset();
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
  pStore->reset();
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

TEST_F(KVStoreTest, simulatedPersistence) {
  pStore->reset();
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
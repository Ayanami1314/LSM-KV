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
    pStore->reset();
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
  ASSERT_EQ(pStore->get(1), "");
  ASSERT_EQ(pStore->get(2), "");

  pStore->put(1, "1");
  pStore->put(2, "2");
  pStore->put(5, "5");

  ASSERT_EQ(pStore->get(1), "1");
  ASSERT_EQ(pStore->get(5), "5");
  ASSERT_EQ(pStore->get(2), "2");
  ASSERT_EQ(pStore->get(3), "");

  pStore->del(1);
  ASSERT_EQ(pStore->get(1), "");
  pStore->put(6, "6");
  pStore->put(7, "7");
  pStore->put(8, "8");
  pStore->del(6);
  ASSERT_EQ(pStore->get(6), "");
  ASSERT_EQ(pStore->get(7), "7");
  ASSERT_EQ(pStore->get(5), "5");

  pStore->reset();
  ASSERT_EQ(pStore->get(8), "");
  ASSERT_EQ(pStore->get(7), "");
}

TEST_F(KVStoreTest, basicScan) {
  pStore->reset();
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
  ASSERT_EQ(res, ref);
  ASSERT_EQ(res2, ref2);
}
TEST_F(KVStoreTest, BasicScan2) {
  pStore->reset();
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;
  int max = 10;
  for (int i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
    pStore->put(i, std::to_string(i));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  ASSERT_EQ(list_ans.size(), list_stu.size());
}
TEST_F(KVStoreTest, BasicScan3) {
  pStore->reset();
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;
  int max = 11;
  for (int i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
    pStore->put(i, std::to_string(i));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  ASSERT_EQ(list_ans, list_stu);
}
TEST_F(KVStoreTest, ScanWithDel) {
  pStore->reset();
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;
  int max = 11;
  for (int i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
    pStore->put(i, std::to_string(i));
  }
  pStore->del(0);
  pStore->scan(0, max / 2 - 1, list_stu);
  ASSERT_EQ(list_ans.size(), list_stu.size() + 1);
  list_ans.pop_front();
  ASSERT_EQ(list_ans, list_stu);
}
TEST_F(KVStoreTest, BasicScan4) {
  pStore->reset();
  pStore->reset();
  pStore->put(1, "SE");
  pStore->del(1);
  auto res = std::list<skiplist::kvpair>();
  auto ref = std::list<skiplist::kvpair>();
  pStore->scan(1, 2, res);
  ASSERT_EQ(res, ref);
}
TEST_F(KVStoreTest, RegularTest50) {
  pStore->reset();

  uint64_t i;
  const std::string not_found = "";
  const int max = 50;
  // Test a single key
  ASSERT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  ASSERT_EQ("SE", pStore->get(1));
  ASSERT_EQ(true, pStore->del(1));
  ASSERT_EQ(not_found, pStore->get(1));
  ASSERT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    ASSERT_EQ(std::to_string(i), pStore->get(i));
  }

  // Test after all insertions
  for (i = 0; i < max; ++i)
    ASSERT_EQ(std::to_string(i), pStore->get(i));

  // Test scan
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;

  for (i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  ASSERT_EQ(list_ans.size(), list_stu.size());

  auto ap = list_ans.begin();
  auto sp = list_stu.begin();
  while (ap != list_ans.end()) {
    if (sp == list_stu.end()) {
      ASSERT_EQ((*ap).first, -1);
      ASSERT_EQ((*ap).second, not_found);
      ap++;
    } else {
      ASSERT_EQ((*ap).first, (*sp).first);
      ASSERT_EQ((*ap).second, (*sp).second);
      ap++;
      sp++;
    }
  }

  // Test deletions
  for (i = 0; i < max; i += 2) {
    ASSERT_EQ(true, pStore->del(i));
  }

  for (i = 0; i < max; ++i)
    ASSERT_EQ((i & 1) ? std::to_string(i) : not_found, pStore->get(i));

  for (i = 1; i < max; ++i)
    ASSERT_EQ(i & 1, pStore->del(i));
}

TEST_F(KVStoreTest, RegularTest200) {
  pStore->reset();
  uint64_t i;
  const std::string not_found = "";
  const int max = 200;
  // Test a single key
  ASSERT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  ASSERT_EQ("SE", pStore->get(1));
  ASSERT_EQ(true, pStore->del(1));
  ASSERT_EQ(not_found, pStore->get(1));
  ASSERT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    ASSERT_EQ(std::to_string(i), pStore->get(i));
  }

  // Test after all insertions
  for (i = 0; i < max; ++i)
    ASSERT_EQ(std::to_string(i), pStore->get(i));

  // Test scan
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;

  for (i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  ASSERT_EQ(list_ans.size(), list_stu.size());

  auto ap = list_ans.begin();
  auto sp = list_stu.begin();
  while (ap != list_ans.end()) {
    if (sp == list_stu.end()) {
      ASSERT_EQ((*ap).first, -1);
      ASSERT_EQ((*ap).second, not_found);
      ap++;
    } else {
      ASSERT_EQ((*ap).first, (*sp).first);
      ASSERT_EQ((*ap).second, (*sp).second);
      ap++;
      sp++;
    }
  }

  // Test deletions
  for (i = 0; i < max; i += 2) {
    ASSERT_EQ(true, pStore->del(i));
  }

  for (i = 0; i < max; ++i)
    ASSERT_EQ((i & 1) ? std::to_string(i) : not_found, pStore->get(i));

  for (i = 1; i < max; ++i)
    ASSERT_EQ(i & 1, pStore->del(i));
}

TEST_F(KVStoreTest, RegularTest1024) {
  pStore->reset();
  uint64_t i;
  const std::string not_found = "";
  const int max = 1024;
  // Test a single key
  ASSERT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  ASSERT_EQ("SE", pStore->get(1));
  ASSERT_EQ(true, pStore->del(1));
  ASSERT_EQ(not_found, pStore->get(1));
  ASSERT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    ASSERT_EQ(std::to_string(i), pStore->get(i));
  }

  // Test after all insertions
  for (i = 0; i < max; ++i)
    ASSERT_EQ(std::to_string(i), pStore->get(i));

  // Test scan
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;

  for (i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
  }

  pStore->scan(0, max / 2 - 1, list_stu);
  ASSERT_EQ(list_ans.size(), list_stu.size());

  auto ap = list_ans.begin();
  auto sp = list_stu.begin();
  while (ap != list_ans.end()) {
    if (sp == list_stu.end()) {
      ASSERT_EQ((*ap).first, -1);
      ASSERT_EQ((*ap).second, not_found);
      ap++;
    } else {
      ASSERT_EQ((*ap).first, (*sp).first);
      ASSERT_EQ((*ap).second, (*sp).second);
      ap++;
      sp++;
    }
  }

  // Test deletions
  for (i = 0; i < max; i += 2) {
    ASSERT_EQ(true, pStore->del(i));
  }

  for (i = 0; i < max; ++i)
    ASSERT_EQ((i & 1) ? std::to_string(i) : not_found, pStore->get(i));

  for (i = 1; i < max; ++i)
    ASSERT_EQ(i & 1, pStore->del(i));
}

TEST_F(KVStoreTest, JustOverflow) {
  pStore->reset();
  uint64_t i;
  const std::string not_found = "";
  const int max = 350;
  // Test a single key
  ASSERT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  ASSERT_EQ("SE", pStore->get(1));
  ASSERT_EQ(true, pStore->del(1));
  ASSERT_EQ(not_found, pStore->get(1));
  ASSERT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    ASSERT_EQ(std::to_string(i), pStore->get(i));
  }

  // // Test after all insertions
  for (i = 0; i < max; ++i)
    ASSERT_EQ(std::to_string(i), pStore->get(i));
}
TEST_F(KVStoreTest, ThreeSST) {
  pStore->reset();
  uint64_t i;
  const std::string not_found = "";
  const int max = 1000;
  // Test a single key
  ASSERT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  ASSERT_EQ("SE", pStore->get(1));
  ASSERT_EQ(true, pStore->del(1));
  ASSERT_EQ(not_found, pStore->get(1));
  ASSERT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    ASSERT_EQ(std::to_string(i), pStore->get(i));
  }

  // // Test after all insertions
  for (i = 0; i < max; ++i)
    ASSERT_EQ(std::to_string(i), pStore->get(i));
}

TEST_F(KVStoreTest, LargeScan) {
  pStore->reset();
  for (int i = 0; i < 1000; ++i) {
    pStore->put(i, std::to_string(i));
  }
  auto ref = std::list<skiplist::kvpair>();
  auto res = std::list<skiplist::kvpair>();
  pStore->scan(100, 100000, res);

  for (int i = 100; i < 1000; ++i) {
    ref.push_back({i, std::to_string(i)});
  }

  ASSERT_EQ(ref, res);
}

TEST_F(KVStoreTest, LargeScanWithDel) {
  pStore->reset();
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

  ASSERT_EQ(ref, res);
}

TEST_F(KVStoreTest, AnotherLargeScan) {
  pStore->reset();
  // Test after all insertions
  uint64_t i;
  auto not_found = "";
  int max = 500;
  // Test a single key
  ASSERT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  ASSERT_EQ("SE", pStore->get(1));
  ASSERT_EQ(true, pStore->del(1));
  ASSERT_EQ(not_found, pStore->get(1));
  ASSERT_EQ(false, pStore->del(1));

  // Test multiple key-value pairs
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
    // ASSERT_EQ(std::to_string(i), pStore->get(i));
  }

  // for (i = 0; i < max; ++i)
  //   ASSERT_EQ(std::to_string(i), pStore->get(i));

  // Test scan
  std::list<std::pair<uint64_t, std::string>> list_ans;
  std::list<std::pair<uint64_t, std::string>> list_stu;

  for (i = 0; i < max / 2; ++i) {
    list_ans.emplace_back(std::make_pair(i, std::to_string(i)));
  }

  pStore->scan(0, max / 2 - 1, list_stu);

  ASSERT_EQ(list_ans.size(), list_stu.size());

  // auto ap = list_ans.begin();
  // auto sp = list_stu.begin();
  // while (ap != list_ans.end()) {
  //   if (sp == list_stu.end()) {
  //     ASSERT_EQ((*ap).first, -1);
  //     ASSERT_EQ((*ap).second, not_found);
  //     ap++;
  //   } else {
  //     ASSERT_EQ((*ap).first, (*sp).first);
  //     ASSERT_EQ((*ap).second, (*sp).second);
  //     ap++;
  //     sp++;
  //   }
  // }
}
TEST_F(KVStoreTest, AnotherLargeScanWithDel) {
  pStore->reset();
  uint64_t i;
  auto not_found = "";
  int max = 345;
  // Test a single key
  ASSERT_EQ(not_found, pStore->get(1));
  pStore->put(1, "SE");
  ASSERT_EQ("SE", pStore->get(1));
  ASSERT_EQ(true, pStore->del(1));
  ASSERT_EQ(not_found, pStore->get(1));
  ASSERT_EQ(false, pStore->del(1));
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i));
  }

  // Test deletions
  for (i = 0; i < max; i += 2) {
    ASSERT_EQ(true, pStore->del(i));
  }

  for (i = 0; i < max; ++i)
    ASSERT_EQ((i & 1) ? std::to_string(i) : not_found, pStore->get(i));

  for (i = 1; i < max; ++i)
    ASSERT_EQ(i & 1, pStore->del(i));
}

TEST_F(KVStoreTest, basicGC) {
  pStore->reset();
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
    ASSERT_EQ(std::to_string(i), pStore->get(i));
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
    ASSERT_EQ(std::string(std::to_string(i) + 's'), pStore->get(i));
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
      ASSERT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      ASSERT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      ASSERT_EQ(std::to_string(i) + 'c', pStore->get(i));
      break;
    default:
      assert(0);
    }
  }
}
TEST_F(KVStoreTest, MidPutOverride) {
  pStore->reset();
  uint64_t i;
  uint64_t gc_trigger = 1024;
  int max = 1024 * 3;
  for (i = 0; i < max; ++i) {
    pStore->put(i, std::to_string(i) + 's');
  }
  std::cout << "Put end." << std::endl;
  for (i = 0; i < max; ++i) {
    ASSERT_EQ(std::string(std::to_string(i) + 's'), pStore->get(i));
    switch (i % 3) {
    case 0:
      pStore->put(i, std::to_string(i) + 'a');
      ASSERT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      pStore->put(i, std::to_string(i) + 'b');
      ASSERT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      pStore->put(i, std::to_string(i) + 'c');
      ASSERT_EQ(std::to_string(i) + 'c', pStore->get(i));
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
      ASSERT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      ASSERT_EQ(std::to_string(i) + 'c', pStore->get(i));
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
    ASSERT_EQ(std::string(std::to_string(i) + 's'), pStore->get(i));
    switch (i % 3) {
    case 0:
      pStore->put(i, std::to_string(i) + 'a');
      ASSERT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      pStore->put(i, std::to_string(i) + 'b');
      ASSERT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      pStore->put(i, std::to_string(i) + 'c');
      ASSERT_EQ(std::to_string(i) + 'c', pStore->get(i));
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
      ASSERT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      ASSERT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      ASSERT_EQ(std::to_string(i) + 'c', pStore->get(i));
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
    ASSERT_EQ(std::string(std::to_string(i) + 's'), pStore->get(i));
    switch (i % 3) {
    case 0:
      pStore->put(i, std::to_string(i) + 'a');
      ASSERT_EQ(std::to_string(i) + 'a', pStore->get(i));
      break;
    case 1:
      pStore->put(i, std::to_string(i) + 'b');
      ASSERT_EQ(std::to_string(i) + 'b', pStore->get(i));
      break;
    case 2:
      pStore->put(i, std::to_string(i) + 'c');

      ASSERT_EQ(std::to_string(i) + 'c', pStore->get(i));
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
      ASSERT_EQ(i % 2 == 0 ? not_found : std::to_string(i) + 'a',
                pStore->get(i));
      break;
    case 1:
      ASSERT_EQ(i % 2 == 0 ? not_found : std::to_string(i) + 'b',
                pStore->get(i));
      break;
    case 2:
      ASSERT_EQ(i % 2 == 0 ? not_found : std::to_string(i) + 'c',
                pStore->get(i));
      break;
    default:
      assert(0);
    }
  }
}
TEST_F(KVStoreTest, largePutAndDel) {
  pStore->reset();
  int max = 1000;
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

TEST_F(KVStoreTest, MultiLayers) {
  pStore->reset();
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
    ASSERT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}
TEST_F(KVStoreTest, smallPersisWithGC) {
  pStore->reset();
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
    ASSERT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}
TEST_F(KVStoreTest, largerPersisWithGC) {
  pStore->reset();
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
    ASSERT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}
TEST_F(KVStoreTest, simulatedPersistenceWithGC) {
  pStore->reset();
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
    ASSERT_EQ(pStore->get(i), i % 2 == 0 ? "" : std::to_string(i));
  }
}
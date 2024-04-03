#include "../sstable.h"
#include "../type.h"
#include <gtest/gtest.h>
class SSTableTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }
  SSTable::sstable_type sstable;
};

TEST_F(SSTableTest, defaultSize) {
  // 32B Header, default 8KB BF
  EXPECT_EQ(sstable.size(), 32 + 8 * 1024);
}

TEST_F(SSTableTest, defaultCalSize) {
  EXPECT_EQ(SSTable::sstable_type::cal_size(10),
            10 * sizeof(kEntry) + 32 + 8 * 1024);
}
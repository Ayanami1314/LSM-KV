#include "../utils.h"
#include "../vlog.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
class vLogTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Code here will be called immediately after the constructor (right
    // before each test).
    vpath = "./tmp/vlog";
    utils::mkdir("./tmp");
    vl = std::make_unique<vLogs>(vpath);
  }

  void TearDown() override {
    // Code here will be called immediately after each test (right
    // before the destructor).
    if (std::filesystem::exists("./tmp")) {
      std::filesystem::remove_all("./tmp");
    }
  }
  TPath vpath;
  std::unique_ptr<vLogs> vl;
};

TEST_F(vLogTest, createFile) {
  vl->clear();
  ASSERT_TRUE(std::filesystem::exists(vpath));
  ASSERT_EQ(std::filesystem::file_size(vpath), 0);
}

TEST_F(vLogTest, addVlog) {
  vl->clear();

  std::ofstream ovfs(vpath, std::ios::binary | std::ios::app);
  ASSERT_TRUE(ovfs.is_open());
  std::string vvalue = "Hello, LSM!";
  vEntryProps v;
  v.key = 0;
  v.vvalue = vvalue;
  v.vlen = vvalue.size();

  vl->addVlog(v, true);
  system("xxd -i ./tmp/vlog");
  auto [ref_vkey, ref_vlen, ref_vvalue] = v;
  auto ref_magic = vl->getMagic();
  TCheckSum ref_checksum = 0;
  vl->cal_bytes(v, ref_checksum);

  ovfs.close();

  std::ifstream ivfs(vpath, std::ios::binary);
  ASSERT_TRUE(ivfs.is_open());
  std::vector<char> buffer((std::istreambuf_iterator<char>(ivfs)),
                           (std::istreambuf_iterator<char>()));
  TMagic magic = 0;
  std::memcpy(&magic, buffer.data(), sizeof(magic));
  EXPECT_EQ(magic, ref_magic);

  TCheckSum checksum = 0;
  std::memcpy(&checksum, buffer.data() + sizeof(magic), sizeof(checksum));
  EXPECT_EQ(checksum, ref_checksum);

  TKey vkey = 0;
  std::memcpy(&vkey, buffer.data() + sizeof(magic) + sizeof(checksum),
              sizeof(vkey));
  EXPECT_EQ(vkey, ref_vkey);
  TLen vlen = 0;
  std::memcpy(&vlen,
              buffer.data() + sizeof(magic) + sizeof(checksum) + sizeof(vkey),
              sizeof(vlen));
  EXPECT_EQ(vlen, ref_vlen);
  EXPECT_EQ(vlen, vvalue.size());

  std::string vvalue_str(buffer.data() + sizeof(magic) + sizeof(checksum) +
                             sizeof(vlen) + sizeof(vkey),
                         ref_vlen);
  EXPECT_EQ(vvalue_str, vvalue);
  EXPECT_EQ(vvalue, ref_vvalue);

  ivfs.close();
}
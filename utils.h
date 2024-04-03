#pragma once

#include "type.h"
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <memory>
#include <queue>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#define PAGE_SIZE (4 * 1024)

template <typename... Args>
std::string string_format(const std::string &format, Args... args) {
  int size =
      snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
  if (size <= 0) {
    throw std::runtime_error("Error during formatting.");
  }
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(buf.get(),
                     buf.get() + size - 1); // We don't want the '\0' inside
}
#define DEBUG 1
#if DEBUG
#define Log(format, ...)                                                       \
  std::cout << string_format(format, ##__VA_ARGS__) << std::endl;
#else
#define Log(format, ...)                                                       \
  do {                                                                         \
  } while (0);
#endif
namespace utils {
/**
 * Check whether directory exists
 * @param path directory to be checked.
 * @return ture if directory exists, false otherwise.
 */
static inline bool dirExists(const std::string &path) {
  struct stat st;
  int ret = stat(path.c_str(), &st);
  return ret == 0 && st.st_mode & S_IFDIR;
}

/**
 * list all filename in a directory
 * @param path directory path.
 * @param ret all files name in directory.
 * @return files number.
 */
static inline int scanDir(const std::string &path,
                          std::vector<std::string> &ret) {
  DIR *dir;
  struct dirent *rent;
  dir = opendir(path.c_str());
  char s[100];
  while ((rent = readdir(dir))) {
    strcpy(s, rent->d_name);
    if (s[0] != '.') {
      ret.push_back(s);
    }
  }
  closedir(dir);
  return ret.size();
}

/**
 * Create directory
 * @param path directory to be created.
 * @return 0 if directory is created successfully, -1 otherwise.
 */
static inline int _mkdir(const std::string &path) {
  return ::mkdir(path.c_str(), 0775);
}

/**
 * Create directory recursively
 * @param path directory to be created.
 * @return 0 if directory is created successfully, -1 otherwise.
 */
static inline int mkdir(const std::string &path) {
  std::string currentPath = "";
  std::string dirName;
  std::stringstream ss(path);

  while (std::getline(ss, dirName, '/')) {
    currentPath += dirName;
    if (!dirExists(currentPath) && _mkdir(currentPath.c_str()) != 0) {
      return -1;
    }
    currentPath += "/";
  }
  return 0;
}

/**
 * Delete a empty directory
 * @param path directory to be deleted.
 * @return 0 if delete successfully, -1 otherwise.
 */
static inline int rmdir(const std::string &path) {
  return ::rmdir(path.c_str());
}

/**
 * Delete a file
 * @param path file to be deleted.
 * @return 0 if delete successfully, -1 otherwise.
 */
static inline int rmfile(const std::string &path) {
  return ::unlink(path.c_str());
}

/**
 * Reclaim space of a file
 * @param path file to be reclaimed.
 * @param offset offset from the beginning of the file you want to start
 * reclaiming at.
 * @param len number of bytes you want to reclaim.
 * @return -1 if fail to open the file, -2 if fallocate fail, and 0 if reclaim
 * space successfully
 * @attention logic size of a file will NOT change after being reclaimed
 * successfully, so always make sure to calculate offset seems like the file has
 * not been reclaimed.
 */
static inline int de_alloc_file(const std::string &path, off_t offset,
                                off_t len) {
  int fd = open(path.c_str(), O_RDWR, 0444);
  if (fd < 0) {
    perror("open");
    return -1;
  }

  // If you want to call fallocate yourself,
  // the code below to handle offset and len is very IMPORTANT!
  // We strongly recommend you to call fallocate yourself only if you are
  // familiar with it!
  len += offset % PAGE_SIZE;
  offset = offset / PAGE_SIZE * PAGE_SIZE;
  if (fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, offset, len) <
      0) {
    perror("fallocate");
    return -2;
  }

  close(fd);

  return 0;
}

/**
 * find the offset of the first block (size of a blcok is 4kB) that has data
 * from the beginning of a file
 * @param path file to be searched for.
 * @return -1 if fail to open, offset if find successfully.
 */
static inline off_t seek_data_block(const std::string &path) {
  int fd = open(path.c_str(), O_RDWR, 0644);
  if (fd < 0) {
    perror("open");
    return -1;
  }
  off_t offset = lseek(fd, 0, SEEK_DATA);
  close(fd);
  return offset;
}

/**
 * util function used by crc16, you needn't call this function yourself
 */
#define POLYNOMIAL 0x1021
static inline std::unique_ptr<uint16_t[]> generate_crc16_table() {
  std::unique_ptr<uint16_t[]> table(new uint16_t[256]{0});
  for (int i = 0; i < 256; i++) {
    uint16_t value = 0;
    uint16_t temp = i << 8;
    for (int j = 0; j < 8; j++) {
      if ((value ^ temp) & 0x8000) {
        value = (value << 1) ^ POLYNOMIAL;
      } else {
        value <<= 1;
      }
      temp <<= 1;
    }
    table[i] = value;
  }
  return table;
}

/**
 * generate crc16
 * @param data binary data used to generate crc16.
 * @return generated crc16.
 */
static inline uint16_t crc16(const std::vector<unsigned char> &data) {
  static const std::unique_ptr<uint16_t[]> crc16_table = generate_crc16_table();
  uint16_t crc = 0xFFFF;
  size_t i = 0, length = data.size();
  while (i < length) {
    crc = (crc << 8) ^ crc16_table[((crc >> 8) ^ data[i++]) & 0xFF];
  }
  return crc;
}
static inline void mergeKSorted(const std::vector<kEntrys> &src, kEntrys &dst) {
  // NOTE: src is sorted by the priority(keep the priority highest at begin!)
  // priority: the time
  // memtable > l0 > l1 > l2 > ..., in the same layer the index is the priority
  // e.g. l0 sst2 < l0 sst4
  // HINT：use priority queue

  std::priority_queue<kEntry> pq;
  int K = src.size();
  std::vector<size_t> limitK(K, 0);
  for (int i = 0; i < K; ++i) {
    limitK[i] = src[i].size();
  }
  std::vector<size_t> index(K, 0);
  for (int i = 0; i < src.size(); ++i) {
    if (!src[i].empty()) {
      pq.push(src[i].front());
      index[i]++;
    }
  }
  TKey lastKey = 0;
  while (!pq.empty()) {
    auto top = pq.top();
    pq.pop();
    if (top.key == lastKey) {
      // NOTE: repeat element should be filtered
      continue;
    }
    dst.push_back(top);
    int i = 0;
    for (; i < K; ++i) {
      if (index[i] < limitK[i]) {
        pq.push(src[i][index[i]]);
        index[i]++;
      }
    }
    lastKey = top.key;
  }
}
} // namespace utils

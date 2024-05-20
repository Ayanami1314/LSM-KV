#include "kvstore.h"
#include "utils.h"
#include <future>
#include <memory>
#include <thread>
using std::cout, std::endl, std::string;
using kvpair = std::pair<std::string, std::string>;
std::atomic<bool> stop_flag(false);
void run(std::function<void(std::atomic<int> &, std::atomic<bool> &flag)> fn,
         string name, int max_seconds) {
  // NOTE：每隔1s统计吞吐量
  std::atomic<int> number(0);

  auto run_fn =
      std::async(std::launch::async, fn, std::ref(number), std::ref(stop_flag));
  for (int i = 0; i < max_seconds; ++i) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << name << " throughput: " << number.load() << " ops/s" << endl
         << std::flush;
    number = 0;
  }
  stop_flag = true;
};
string random_str(int n) {
  string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  string new_str;
  for (int i = 0; i < n; ++i) {
    new_str.push_back(str[rand() % str.size()]);
  }
  return new_str;
};
int main(int argc, const char *argv[]) {
  if (argc != 3) {
    cout << "Usage: ./put-plot-test <put-size> <max_seconds>" << endl;
    return 0;
  }

  string sst_path = "./put-plot/data";
  string vlog_path = "./put-plot/vlog";
  if (utils::dirExists(sst_path)) {
    std::string cmd = "sudo rm -rf " + sst_path;
    system(cmd.c_str());
  }
  if (utils::dirExists(vlog_path)) {
    std::string cmd = "sudo rm -rf " + vlog_path;
    system(cmd.c_str());
  }
  std::shared_ptr<KVStore> kvs = std::make_shared<KVStore>(sst_path, vlog_path);
  const int put_size = atoi(argv[1]);
  const int max_seconds = atoi(argv[2]);
  std::vector<std::pair<int, string>> test_kvs;
  auto put_fn = [&kvs, put_size](std::atomic<int> &number,
                                 std::atomic<bool> &stop_flag) {
    while (!stop_flag) {
      kvs->put(number.load(), random_str(put_size));
      number++;
    }
  };

  run(put_fn, "put", max_seconds);
  // delete all files
  if (utils::dirExists(sst_path)) {
    std::string cmd = "sudo rm -rf " + sst_path;
    system(cmd.c_str());
  }
  if (utils::dirExists(vlog_path)) {
    std::string cmd = "sudo rm -rf " + vlog_path;
    system(cmd.c_str());
  }
  return 0;
}
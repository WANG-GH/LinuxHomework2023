#include "spliter.h"
#include "sort_mgr.h"
#include "task.h"
#include "thread_pool.h"
#include <gtest/gtest.h>
#include <queue>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void delete_directory(const std::string& dir_path) {
    if (fs::exists(dir_path)) {
        fs::remove_all(dir_path);
    }
}

TEST(MergeSort, DISABLED_FileSortTest) {
  SortTask t("/home/wyy/prog/linux-homework/homework3/unsorted_data/tmp1",
             "/home/wyy/prog/linux-homework/homework3/unsorted_data/tmp2",
             "/home/wyy/prog/linux-homework/homework3/target");
  t.sort();
}

TEST(MergeSort, DISABLED_ThreadsTest) {
  fixed_thread_pool tp(3);

  std::queue<std::string> files;
  files.push("/home/wyy/prog/linux-homework/homework3/unsorted_data/tmp1");
  files.push("/home/wyy/prog/linux-homework/homework3/unsorted_data/tmp2");
  // files.push("/home/wyy/prog/linux-homework/homework3/unsorted_data/tmp3");

  tp.set_files(files);
  tp.wait();
}

TEST(MergeSort, DISABLED_SplitFileTest) {
  // 打开目录，将大文件分割成多个k行的小文件
  string output_dir = "/home/wyy/prog/linux-homework/homework3/output_dir";
  delete_directory(output_dir);

  Spliter sp("/home/wyy/prog/linux-homework/homework3/input_dir",
             output_dir, 4);

  sp.split_files();
  ASSERT_FALSE(sp.get_spilted_files_names().empty());
}

TEST(MergeSort, DISABLED_AllComposeTest){
  // 打开目录，将大文件分割成多个k行的小文件
  string output_dir = "/home/wyy/prog/linux-homework/homework3/output_dir";
  delete_directory(output_dir);

  Spliter sp("/home/wyy/prog/linux-homework/homework3/input_dir",
             output_dir, 4);

  sp.split_files();

  fixed_thread_pool tp(3);

  tp.set_files(sp.get_spilted_files_names());

  tp.wait();
}

TEST(MergeSort, SortMgrTest){
  SortMgr mgr("/home/wyy/prog/linux-homework/homework3/input_dir");
  mgr.start();
}

int main(int argc, char **avgv) {
  ::testing::InitGoogleTest(&argc, avgv);
  return RUN_ALL_TESTS();
}

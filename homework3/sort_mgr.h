#pragma once
#include "spliter.h"
#include "thread_pool.h"
#include <string>

using namespace std;
class SortMgr{
public:
  explicit SortMgr(string source_dir): source_dir_(source_dir), tp_(fixed_thread_pool(10)){
    sp_.set_data(source_dir, "tmp", 4);
  }
  void start(){
    sp_.split_files();
    tp_.set_files(sp_.get_spilted_files_names());
    tp_.wait();
  }
private:
  std::string source_dir_;
  Spliter sp_;
  fixed_thread_pool tp_;
};
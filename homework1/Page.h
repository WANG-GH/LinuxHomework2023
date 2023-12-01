#pragma once
#include <cstring>

#define PAGE_SIZE 4

struct Page{
public:
  Page(){
    memset(data_, 0, PAGE_SIZE);
  }
  ~Page() = default;

public:
  char data_[PAGE_SIZE]{};
  int page_id_;
  int valid_size_;
  bool is_dirty_;
};
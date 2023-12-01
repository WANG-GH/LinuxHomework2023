#pragma once

#include "Page.h"
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include <memory>

class Wrapper{
public:
  Wrapper(int flush_size):flush_size_(flush_size){}
  Wrapper() = default;
  ~Wrapper() = default;

  int open(const char *pathname, int flags);
  ssize_t write(const void *buf, size_t count);
  ssize_t read(void *buf, size_t count);
  // 当前只支持seek set
  off_t lseek(off_t offset);
  int close();

private:
  void flush(int id);
  void flush_all();

  // 该函数封装了刷新策略，上层只需要调用这个函数拿到PID就好
  inline int get_page_id(int offset);
  std::shared_ptr<Page> get_page(int page_id);  // 当当前缓冲区的数目大于四时，刷新所有缓冲，再返回页面

private:
  // page_size * page_id, Page
  std::unordered_map<int, std::shared_ptr<Page>> buffer_pool_;
  int fd_{-1};
  int file_offset_{0};
  int file_end_{0};
  int flush_size_{4};
};
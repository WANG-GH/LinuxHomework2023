#include "Wrapper.h"
#include <algorithm>
#include <cassert>
#include <fcntl.h>

int Wrapper::open(const char *pathname, int flags) {
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  int fd = ::open(pathname, flags, mode);
  if (fd < 0) {
    return fd;
  }
  fd_ = fd;
  file_end_ = ::lseek(fd_, 0, SEEK_END);
  ::lseek(fd_, 0, SEEK_SET);

  return fd_;
}

// 写的话肯定能写完
ssize_t Wrapper::write(const void *buf, size_t count) {
  assert(fd_ >= 0);
  int all_size = 0;

  while (all_size < count) {
    int pid = get_page_id(file_offset_);
    auto p = get_page(pid);

    int pg_offset = file_offset_ % PAGE_SIZE;
    int pg_remain = PAGE_SIZE - pg_offset;
    int count_remain = count - all_size;

    int write_size = 0;
    if (pg_offset + count_remain < PAGE_SIZE) {
      // 一页能放下
      write_size = count_remain;
    } else {
      // 一页放不下
      write_size = pg_remain;
    }

    memcpy(p->data_ + pg_offset, (char *)buf + all_size, write_size);

    file_offset_ += write_size;
    all_size += write_size;
    file_end_ = std::max(file_end_, file_offset_);
  }
  return all_size;
}

ssize_t Wrapper::read(void *buf, size_t count) {
  assert(fd_ >= 0);
  if (file_offset_ > file_end_) {
    return 0;
  }

  int count_end = file_offset_ + count;
  int all_size = 0;
  int end_pid = get_page_id(file_end_);

  while (all_size < count) {
    int pid = get_page_id(file_offset_);
    auto p = get_page(pid);

    int pg_offset = file_offset_ % PAGE_SIZE;
    int read_size = 0;

    if (pid < end_pid) {
      // 没到文件尾的页，当前页全部都可以读出来
      read_size = PAGE_SIZE - pg_offset;
      memcpy((char *)buf + all_size, p->data_ + pg_offset, read_size);

      all_size += read_size;
      file_offset_ += read_size;
    } else {
      // 文件尾在该页，要判断是否和文件尾部冲突
      int pg_valid = file_end_ % PAGE_SIZE;
      int read_remain = count - all_size;
      if (pg_offset + read_remain < pg_valid) {
        // 不冲突
        read_size = read_remain;
        memcpy((char *)buf + all_size, p->data_ + pg_offset, read_size);
      } else {
        // 冲突，需要截断
        read_size = pg_valid - pg_offset;
        memcpy((char *)buf + all_size, p->data_ + pg_offset, read_size);
      }

      file_offset_ += read_size;
      all_size += read_size;
      break;
    }
  }

  return all_size;
}

off_t Wrapper::lseek(off_t offset) {
  assert(fd_ >= 0);

  file_offset_ = offset;
  return 0;
}

int Wrapper::close() {
  assert(fd_ >= 0);
  flush_all();
  return ::close(fd_);
}

int Wrapper::get_page_id(int offset) { return offset / PAGE_SIZE; }

std::shared_ptr<Page> Wrapper::get_page(int page_id) {
  if (buffer_pool_.count(page_id)) {
    return buffer_pool_[page_id];
  }

  if (buffer_pool_.size() >= flush_size_) {
    flush_all();
  }

  auto p = std::make_shared<Page>();
  int offset = page_id * PAGE_SIZE;
  ::lseek(fd_, offset, SEEK_SET);
  ::read(fd_, p->data_, PAGE_SIZE);
  buffer_pool_[page_id] = p;
  return p;
}

void Wrapper::flush_all() {
  int end_pid = get_page_id(file_end_);

  for (auto &[pid, page] : buffer_pool_) {
    int tmp_offset = pid * PAGE_SIZE;
    ::lseek(fd_, tmp_offset, SEEK_SET);

    if (pid < end_pid) {
      ::write(fd_, page->data_, PAGE_SIZE);
    } else {
      ::write(fd_, page->data_, file_end_ - tmp_offset);
    }
  }

  buffer_pool_.clear();
}
#pragma once

#include "task.h"
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

class fixed_thread_pool {
public:
  explicit fixed_thread_pool(size_t thread_count)
      : data_(std::make_shared<data>()) {
    for (size_t i = 0; i < thread_count; ++i) {
      std::thread([data = data_] {
        std::unique_lock<std::mutex> lk(data->mtx_);
        for (;;) {
          if (data->files_.size() >= 2) {
            // 领取任务
            auto file_name1 = data->files_.front();
            data->files_.pop();
            auto file_name2 = data->files_.front();
            data->files_.pop();
            auto target_name = "tmp" + std::to_string(data->file_id_);

            data->file_id_++;
            data->running_++;
            lk.unlock();

            // 执行任务
            SortTask t(file_name1, file_name2, target_name);
            t.sort();

            // 将执行完的文件放入队列中
            lk.lock();
            data->running_--;
            data->files_.push(target_name);
          } else if (data->is_shutdown_) {
            break;
          } else if (data->files_.empty()) {
            //还没有分配任务，wait等待任务分配
            std::cout << "sleep no task: " << std::this_thread::get_id() << std::endl;
            data->cond_.wait(lk);
          } else {
            if (data->running_ > 0) {
              std::cout << "sleep files<2 && some thread is running: " << std::this_thread::get_id() << std::endl;
              data->cond_.wait(lk);
            } else {
              assert(data->files_.size() == 1);
              assert(data->running_ == 0);
              std::cout << "last thread finish: " << std::this_thread::get_id() << std::endl;
              data->cond_.notify_one();
              break;
            }
          }
        }
      }).detach();
    }
  }

  fixed_thread_pool() = default;
  fixed_thread_pool(fixed_thread_pool &&) = default;

  ~fixed_thread_pool() {
    if ((bool)data_) {
      {
        std::lock_guard<std::mutex> lk(data_->mtx_);
        data_->is_shutdown_ = true;
      }
      data_->cond_.notify_all();
    }
  }

  void wait() {
    std::unique_lock<std::mutex> lk(data_->mtx_);
    data_->cond_.wait(lk, [this] {
      return (data_->running_ == 0 && data_->files_.size() == 1);
    });
    data_->is_shutdown_ = true;
    data_->cond_.notify_all();
    assert(data_->files_.size() == 1);
    std::rename(data_->files_.front().c_str(), "output_file");
    std::cout << "wait finish! notifiy all: " <<  std::endl;
  }

  void set_files(std::queue<std::string> files) {
    {
      std::lock_guard lg(data_->mtx_);
      data_->files_.swap(files);
    }
    data_->cond_.notify_all();
  }

private:
  struct data {
    std::mutex mtx_;
    std::condition_variable cond_;
    bool is_shutdown_ = false;
    std::queue<std::string> files_;
    std::atomic<int> running_{0};
    std::atomic<int> file_id_{100};
  };
  std::shared_ptr<data> data_;
};
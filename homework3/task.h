#pragma once
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
using namespace std;

class SortTask {
public:
  SortTask(std::string file_name1, std::string file_name2,
           std::string target_name)
      : file_name1_(file_name1), file_name2_(file_name2) {
    target_name_ = target_name;
  }

  void sort() {
    std::cout << std::this_thread::get_id() << ": start sort: " << file_name1_
              << " && " << file_name2_ << std::endl;

    std::ifstream f1(file_name1_), f2(file_name2_);
    ofstream tf(target_name_);
    long num1, num2;

    if (!(f1 >> num1) || !(f2 >> num2)) {
      assert(0);
    }

    bool end1 = false, end2 = false;
    while (!end1 && !end2) {
      if (num1 < num2) {
        tf << num1 << endl;
        end1 = (f1 >> num1) ? false : true;
      } else {
        tf << num2 << endl;
        end2 = (f2 >> num2) ? false : true;
      }
    }

    while (!end1) {
      tf << num1 << endl;
      end1 = (f1 >> num1) ? false : true;
    }
    while (!end2) {
      tf << num2 << endl;
      end2 = (f2 >> num2) ? false : true;
    }

    f1.close();
    f2.close();
    remove(file_name1_.c_str());
    remove(file_name2_.c_str());
    tf.close();
  }

private:
  std::string file_name1_;
  std::string file_name2_;
  std::string target_name_;
};

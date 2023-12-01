#pragma once

#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

class Spliter{
public:
  Spliter() = default;
  Spliter(string input_dir, string output_dir, int max_lines):input_dir_(input_dir), output_dir_(output_dir), max_lines_(max_lines){
    if (!fs::exists(output_dir)) {
        fs::create_directory(output_dir);
    }
  }
  
  void set_data(string input_dir, string output_dir, int max_lines){
    input_dir_ = input_dir;
    output_dir_ = output_dir;
    max_lines_ = max_lines;

    if (fs::exists(output_dir)) {
        fs::remove_all(output_dir);
    }
    fs::create_directory(output_dir);
  }

  void split_files();
  queue<string> get_spilted_files_names(){
    return spilted_files_names_;
  }

private:
  void split_file(const string& filename);

  queue<string> spilted_files_names_;
  string input_dir_;
  string output_dir_;
  int max_lines_;
};

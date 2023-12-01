#include "spliter.h"
#include <algorithm>

void Spliter::split_file(const string &filename) {
  ifstream input(filename);
  if (!input.is_open()) {
    cerr << "Failed to open file: " << filename << endl;
    return;
  }

  string line;
  int line_count = 0;
  int file_count = 0;
  ofstream output;

  vector<long> datas;

  while (getline(input, line)) {
    if (line_count == 0) {
      if (output.is_open()) {
        sort(datas.begin(), datas.end());
        for (auto data : datas) {
          output << data << endl;
        }

        output.close();
        datas.clear();
      }
      string output_filename = output_dir_ + "/" +
                               fs::path(filename).filename().string() + "." +
                               to_string(file_count++);
      spilted_files_names_.push(output_filename);
      output.open(output_filename);
      if (!output.is_open()) {
        cerr << "Failed to create file: " << output_filename << endl;
        return;
      }
    }
    // output << line << endl;
    long data = std::stol(line);
    datas.push_back(data);
    line_count++;
    if (line_count == max_lines_) {
      line_count = 0;
    }
  }

  if (output.is_open()) {
    sort(datas.begin(), datas.end());
    for (auto data : datas) {
      output << data << endl;
    }

    output.close();
    datas.clear();
  }
}

void Spliter::split_files() {
  for (const auto &entry : fs::directory_iterator(input_dir_)) {
    if (entry.is_regular_file()) {
      split_file(entry.path().string());
    }
  }
}
#include "Wrapper.h"
#include <gtest/gtest.h>
#include <stdio.h>

#include <iostream>
#include <random>
#include <string>

std::string generateRandomString(int length) {
  std::string str(length, 0);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 25);
  for (int i = 0; i < length; i++) {
    str[i] = 'a' + dis(gen);
  }
  return str;
}
std::string generatePageString(int pages){
  std::stringstream ss;
  for(int i = 0; i<pages; i++){
    for(int j = 0; j < PAGE_SIZE; j++)
      ss<< std::to_string(i);
  }
  return ss.str();
}

TEST(WrapperTest, BasicReadWriteTest) {
  Wrapper w;
  w.open("file1.txt", O_CREAT | O_RDWR);
  char buf1[3] = "12";
  w.write(buf1, 3);

  char buf1_t[3];
  w.lseek(0);
  w.read(buf1_t, 3);
  w.close();
  ASSERT_EQ(strcmp(buf1, buf1_t), 0);

  ::remove("file1.txt");
}

TEST(WrapperTest, ReadWriteCrossPage) {
  Wrapper w(2);
  w.open("WriteCrossPage.txt", O_CREAT | O_RDWR);
  char buf1[6] = "12345";
  w.write(buf1, 6);

  char buf1_t[6];
  w.lseek(0);
  w.read(buf1_t, 6);
  w.close();
  ASSERT_EQ(strcmp(buf1, buf1_t), 0);

  ::remove("WriteCrossPage.txt");
}

TEST(WrapperTest, ReadBelowFileEnd) {
  Wrapper w(2);
  w.open("ReadBelowFileEnd.txt", O_CREAT | O_RDWR);
  char buf1[6] = "12345";
  w.write(buf1, 6);

  char buf1_t[6];
  w.lseek(10);
  auto size = w.read(buf1_t, 6);
  w.close();
  ASSERT_EQ(size, 0);

  ::remove("ReadBelowFileEnd.txt");
}

TEST(WrapperTest, ReadBiggerThanFileEnd) {
  Wrapper w(2);
  w.open("ReadBiggerThanFileEnd.txt", O_CREAT | O_RDWR);
  char buf1[6] = "12345";
  w.write(buf1, 6);

  char buf1_t[6];
  w.lseek(2);
  auto size = w.read(buf1_t, 100);
  w.close();
  ASSERT_EQ(size, 4);

  ::remove("ReadBiggerThanFileEnd.txt");
}

TEST(WrapperTest, Cross3Pages) {
  Wrapper w(2);
  w.open("Cross3Pages.txt", O_CREAT | O_RDWR);
  const int str_size = 12;
  // std::string big_str = generateRandomString(str_size);
  std::string big_str = "111122223333";
  std::stringstream ss;
  for (const auto &item : big_str) {
    ss << std::hex << int(item);
  }
  std::string s2 = ss.str();

  ASSERT_EQ(w.write(big_str.c_str(), str_size), str_size);

  char buf1_t[str_size];
  w.lseek(0);
  ASSERT_EQ(w.read(buf1_t, str_size), str_size);
  w.close();

  ::remove("Cross3Pages.txt");
  ASSERT_EQ(strcmp(big_str.c_str(), buf1_t), 0);
}


TEST(WrapperTest, BigReadWrite) {
  Wrapper w(4);
  w.open("BigReadWrite.txt", O_CREAT | O_RDWR);
  std::string big_str = generateRandomString(2<<20);
  ASSERT_EQ(w.write(big_str.c_str(), big_str.size()), big_str.size());

  char buf1_t[big_str.size()];
  w.lseek(0);
  ASSERT_EQ(w.read(buf1_t, big_str.size()), big_str.size());
  w.close();

  ASSERT_EQ(strncmp(big_str.c_str(), buf1_t, big_str.size()), 0);
}

TEST(WrapperTest, FileHole) {
  Wrapper w(2);
  w.open("FileHole.txt", O_CREAT | O_RDWR);
  w.lseek(8);
  char buf1[3] = "12";
  w.write(buf1, 3);

  char buf1_t[3];
  w.lseek(0);
  auto size = w.read(buf1_t, 4);
  w.close();
  ASSERT_EQ(size, 4);

  ::remove("FileHole.txt");
}

int main(int argc, char **avgv) {
  ::testing::InitGoogleTest(&argc, avgv);
  return RUN_ALL_TESTS();
}
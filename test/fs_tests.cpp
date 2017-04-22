#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

#include "../src/fs.h"

using namespace std;

class FSTest : public ::testing::Test {
protected:
  FSTest() {
    // system("rm -f disk0");
    // system("rm -f disk1");
    // system("./create_fs disk0");
    // system("./create_fs disk1");
  }
};

// test create_file return code 1 for success
// TEST_F(FSTest, create_file_test) {
// 	myFileSystem f ((char*) "disk0");
// 	int code = f.create_file((char*)"test.c",8);
// 	ASSERT_EQ(1,code);
// }
//
// TEST_F(FSTest, ls_file_test) {
// 	myFileSystem f ((char*) "disk0");
// 	f.ls();
// }
//
// TEST_F(FSTest, ls_file_write) {
// 	myFileSystem f ((char*) "disk0");
//   char temp[1024] = "This is a test for write";
// 	int code = f.write((char*)"test.c",2,temp);
//   ASSERT_EQ(1,code);
// }
//
// TEST_F(FSTest, ls_file_read) {
// 	myFileSystem f ((char*) "disk0");
//   char temp[1024];
// 	int code = f.read((char*)"test.c",2,temp);
//   std::cout << temp << '\n';
//   ASSERT_EQ(1,code);
// }

TEST_F(FSTest, create_file_test_max) {
	myFileSystem f ((char*) "disk0");
  for (int i = 0; i < 16; i++) {
    f.create_file((char*)"test.c",8);
  }
}
// test delete_file return code -1 for failure
// TEST_F(FSTest, delete_file_test) {
// 	myFileSystem f ((char*) "disk0");
// 	int code = f.delete_file((char*)"test.c");
// 	ASSERT_EQ(1,code);
// }




int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

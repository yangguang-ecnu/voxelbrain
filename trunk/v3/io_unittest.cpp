#include <gtest/gtest.h>

#include "io.h"

//goals
// 1. Simplicity.
// 2. Speed.
// 3. Ease of reading/writing binary formats.

//everything is read into a buffer.

using namespace std;

void write(Io & in){
  int a;
  Io failure("");
  failure.GetInt(&a); EXPECT_EQ(false, failure.valid());

  EXPECT_EQ( 0, in.size()); //what's inside
  EXPECT_EQ( 0, in.get_position());
  in.PutInt(10).PutFloat(20.0f);
  EXPECT_EQ( 8, in.get_position());
  in.rewind(); EXPECT_EQ( 0, in.get_position());
};

void read(Io & in){
  float f;
  int i;
  EXPECT_EQ(8, in.size()); //what's inside
  EXPECT_EQ(0, in.get_position());
  in.GetInt(&i).GetFloat(&f);
  EXPECT_EQ(10, i);
  EXPECT_EQ(20.0f, f);
  EXPECT_EQ(8, in.get_position());
  in.rewind(); EXPECT_EQ(0, in.get_position());
  in.set_position(in.get_position()+1);
  EXPECT_EQ(1, in.get_position());
};

TEST(Io, Serialization){
  Io in("");
  write(in);
  std::string copy = in.content();
  Io io(""); io.ReplaceContent(copy);  
  EXPECT_EQ(0, io.get_position()); //Check that the content is rewound
  read(in);
  read(io);

  EXPECT_TRUE(io);
  io.set_position(-1); EXPECT_FALSE(io);
  in.set_position(100); EXPECT_FALSE(in);
};

TEST(Io, File){
  std::string in = ReadFile("lh.pial");
  std::string none = ReadFile("lh_unexistent.pial");
  int string_size = in.size();
  EXPECT_EQ(5505349, string_size);
  EXPECT_EQ(0, none.size());
  int result;
  result = system("rm lh_test.pial");
  EXPECT_TRUE(WriteFile("lh_test.pial", in));
  std::string test = ReadFile("lh_test.pial");
  EXPECT_EQ(string_size, test.size());
};

/*
Trying basic IO

 */
TEST(Io, Types){ //+
  Io io("");
  io.PutChar(23);
  io.PutFloat(1.0f);
  io.PutShort(378);
  io.PutInt(65537);
  io.rewind();
  EXPECT_EQ(23, io.GetChar()) << "Pointer at " << io.get_position();
  EXPECT_FLOAT_EQ(1.0f, io.GetFloat()) << "Pointer at " << io.get_position();
  EXPECT_FLOAT_EQ(378, io.GetShort()) << "Pointer at " << io.get_position();
  EXPECT_FLOAT_EQ(65537, io.GetInt()) << "Pointer at " << io.get_position();
};

TEST(Io, DirectIntefrace){
  Io io(""); short test;
  EXPECT_EQ(0, io.get_position());
  io.PutChar(23).PutChar(32).PutShort(256).PutShort(1);
  io.rewind();
  EXPECT_EQ(23, io.GetChar()) << "Pointer at " << io.get_position();
  EXPECT_EQ(32, io.GetChar());
  EXPECT_FLOAT_EQ(256, io.GetShort()) << "Pointer at " << io.get_position();
  io.GetShort(&test);
  EXPECT_FLOAT_EQ(1, test);
  EXPECT_TRUE(io.valid());
};

TEST(Io, GzipFile){
  string file_name("io_gzipfile.gz");
  string test_string("Hi there");
  WriteGzipFile(file_name, test_string);
  string test_string_read = ReadGzipFile(file_name);
  EXPECT_EQ(test_string, test_string_read);
  string test_string_empty = ReadGzipFile("Not_existing_file.with_extension");
  EXPECT_EQ(test_string_empty, string(""));
};

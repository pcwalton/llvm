//===- llvm/unittest/Support/raw_ostream_test.cpp - raw_ostream tests -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include <float.h>

using namespace llvm;

namespace {

template<typename T> std::string printToString(const T &Value) {
  std::string res;
  llvm::raw_string_ostream(res) << Value;
  return res;    
}

/// printToString - Print the given value to a stream which only has \arg
/// BytesLeftInBuffer bytes left in the buffer. This is useful for testing edge
/// cases in the buffer handling logic.
template<typename T> std::string printToString(const T &Value,
                                               unsigned BytesLeftInBuffer) {
  // FIXME: This is relying on internal knowledge of how raw_ostream works to
  // get the buffer position right.
  SmallString<256> SVec;
  assert(BytesLeftInBuffer < 256 && "Invalid buffer count!");
  llvm::raw_svector_ostream OS(SVec);
  unsigned StartIndex = 256 - BytesLeftInBuffer;
  for (unsigned i = 0; i != StartIndex; ++i)
    OS << '?';
  OS << Value;
  return OS.str().substr(StartIndex);
}

template<typename T> std::string printToStringUnbuffered(const T &Value) {
  std::string res;
  llvm::raw_string_ostream OS(res);
  OS.SetUnbuffered();
  OS << Value;
  return res;
}

TEST(raw_ostreamTest, Types_Buffered) {
  // Char
  EXPECT_EQ("c", printToString('c'));

  // String
  EXPECT_EQ("hello", printToString("hello"));
  EXPECT_EQ("hello", printToString(std::string("hello")));

  // Int
  EXPECT_EQ("0", printToString(0));
  EXPECT_EQ("2425", printToString(2425));
  EXPECT_EQ("-2425", printToString(-2425));

  // Long long
  EXPECT_EQ("0", printToString(0LL));
  EXPECT_EQ("257257257235709", printToString(257257257235709LL));
  EXPECT_EQ("-257257257235709", printToString(-257257257235709LL));

  // Double
  EXPECT_EQ("1.100000e+00", printToString(1.1));
  EXPECT_EQ("0.000000e+00", printToString(0.0));
  EXPECT_EQ("-0.000000e+00", printToString(-0.0));
  EXPECT_EQ("2.225074e-308", printToString(DBL_MIN));
  EXPECT_EQ("1.797693e+308", printToString(DBL_MAX));
  EXPECT_EQ("2.220446e-16", printToString(DBL_EPSILON));

  // 54B_249AD06E636CE
  EXPECT_EQ("9.999999e+99",  printToString(9.9999990e+99));
  // 54B_249AD163D7D25
  EXPECT_EQ("9.999999e+99",  printToString(9.9999995e+99));
  // 54B_249AD163D7D26
  const double d9 = pow(2.0, 0x54B - 0x3FF - 52);
  EXPECT_EQ("1.000000e+100", printToString(9.9999995e+99 + d9));
  // 54B_249AD2594C37D
  EXPECT_EQ("1.000000e+100", printToString(1e100));

  // Denormalized
  const double dm = DBL_MIN / 0x10000000000000LL;
  // 000_0000000000001
  EXPECT_EQ("4.940656e-324", printToString(dm));
  // 000_8000000000000
  EXPECT_EQ("1.112537e-308", printToString(DBL_MIN / 2.0));
  // 000_FFFFFFFFFFFFF
  EXPECT_EQ("2.225074e-308", printToString(DBL_MIN - dm));

  // void*
  EXPECT_EQ("0x0", printToString((void*) 0));
  EXPECT_EQ("0xbeef", printToString((void*) 0xbeef));
  EXPECT_EQ("0xdeadbeef", printToString((void*) 0xdeadbeef));

  // Min and max.
  EXPECT_EQ("18446744073709551615", printToString(UINT64_MAX));
  EXPECT_EQ("-9223372036854775808", printToString(INT64_MIN));
}

TEST(raw_ostreamTest, Types_Unbuffered) {  
  // Char
  EXPECT_EQ("c", printToStringUnbuffered('c'));

  // String
  EXPECT_EQ("hello", printToStringUnbuffered("hello"));
  EXPECT_EQ("hello", printToStringUnbuffered(std::string("hello")));

  // Int
  EXPECT_EQ("0", printToStringUnbuffered(0));
  EXPECT_EQ("2425", printToStringUnbuffered(2425));
  EXPECT_EQ("-2425", printToStringUnbuffered(-2425));

  // Long long
  EXPECT_EQ("0", printToStringUnbuffered(0LL));
  EXPECT_EQ("257257257235709", printToStringUnbuffered(257257257235709LL));
  EXPECT_EQ("-257257257235709", printToStringUnbuffered(-257257257235709LL));

  // Double
  EXPECT_EQ("1.100000e+00", printToStringUnbuffered(1.1));
  EXPECT_EQ("0.000000e+00", printToStringUnbuffered(0.0));
  EXPECT_EQ("-0.000000e+00", printToStringUnbuffered(-0.0));
  EXPECT_EQ("2.225074e-308", printToStringUnbuffered(DBL_MIN));
  EXPECT_EQ("1.797693e+308", printToStringUnbuffered(DBL_MAX));
  EXPECT_EQ("2.220446e-16", printToStringUnbuffered(DBL_EPSILON));

  // 54B_249AD06E636CE
  EXPECT_EQ("9.999999e+99",  printToStringUnbuffered(9.9999990e+99));
  // 54B_249AD163D7D25
  EXPECT_EQ("9.999999e+99",  printToStringUnbuffered(9.9999995e+99));
  // 54B_249AD163D7D26
  const double d9 = pow(2.0, 0x54B - 0x3FF - 52);
  EXPECT_EQ("1.000000e+100", printToStringUnbuffered(9.9999995e+99 + d9));
  // 54B_249AD2594C37D
  EXPECT_EQ("1.000000e+100", printToStringUnbuffered(1e100));

  // Denormalized
  const double dm = DBL_MIN / 0x10000000000000LL;
  // 000_0000000000001
  EXPECT_EQ("4.940656e-324", printToStringUnbuffered(dm));
  // 000_8000000000000
  EXPECT_EQ("1.112537e-308", printToStringUnbuffered(DBL_MIN / 2.0));
  // 000_FFFFFFFFFFFFF
  EXPECT_EQ("2.225074e-308", printToStringUnbuffered(DBL_MIN - dm));

  // void*
  EXPECT_EQ("0x0", printToStringUnbuffered((void*) 0));
  EXPECT_EQ("0xbeef", printToStringUnbuffered((void*) 0xbeef));
  EXPECT_EQ("0xdeadbeef", printToStringUnbuffered((void*) 0xdeadbeef));

  // Min and max.
  EXPECT_EQ("18446744073709551615", printToStringUnbuffered(UINT64_MAX));
  EXPECT_EQ("-9223372036854775808", printToStringUnbuffered(INT64_MIN));
}

TEST(raw_ostreamTest, BufferEdge) {  
  EXPECT_EQ("1.20", printToString(format("%.2f", 1.2), 1));
  EXPECT_EQ("1.20", printToString(format("%.2f", 1.2), 2));
  EXPECT_EQ("1.20", printToString(format("%.2f", 1.2), 3));
  EXPECT_EQ("1.20", printToString(format("%.2f", 1.2), 4));
  EXPECT_EQ("1.20", printToString(format("%.2f", 1.2), 10));
}

TEST(raw_ostreamTest, TinyBuffer) {
  std::string Str;
  raw_string_ostream OS(Str);
  OS.SetBufferSize(1);
  OS << "hello";
  OS << 1;
  OS << 'w' << 'o' << 'r' << 'l' << 'd';
  EXPECT_EQ("hello1world", OS.str());
}

TEST(raw_ostreamTest, WriteEscaped) {
  std::string Str;

  Str = "";
  raw_string_ostream(Str).write_escaped("hi");
  EXPECT_EQ("hi", Str);

  Str = "";
  raw_string_ostream(Str).write_escaped("\\\t\n\"");
  EXPECT_EQ("\\\\\\t\\n\\\"", Str);

  Str = "";
  raw_string_ostream(Str).write_escaped("\1\10\200");
  EXPECT_EQ("\\001\\010\\200", Str);
}

}

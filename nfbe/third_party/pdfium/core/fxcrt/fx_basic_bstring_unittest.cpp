// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/include/fx_string.h"
#include "testing/fx_string_testhelpers.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(fxcrt, ByteStringOperatorSubscript) {
  // CFX_ByteString includes the NUL terminator for non-empty strings.
  CFX_ByteString abc("abc");
  EXPECT_EQ('a', abc[0]);
  EXPECT_EQ('b', abc[1]);
  EXPECT_EQ('c', abc[2]);
  EXPECT_EQ(0, abc[3]);
}

TEST(fxcrt, ByteStringOperatorLT) {
  CFX_ByteString empty;
  CFX_ByteString a("a");
  CFX_ByteString abc("abc");
  CFX_ByteString def("def");

  EXPECT_FALSE(empty < empty);
  EXPECT_FALSE(a < a);
  EXPECT_FALSE(abc < abc);
  EXPECT_FALSE(def < def);

  EXPECT_TRUE(empty < a);
  EXPECT_FALSE(a < empty);

  EXPECT_TRUE(empty < abc);
  EXPECT_FALSE(abc < empty);

  EXPECT_TRUE(empty < def);
  EXPECT_FALSE(def < empty);

  EXPECT_TRUE(a < abc);
  EXPECT_FALSE(abc < a);

  EXPECT_TRUE(a < def);
  EXPECT_FALSE(def < a);

  EXPECT_TRUE(abc < def);
  EXPECT_FALSE(def < abc);
}

TEST(fxcrt, ByteStringOperatorEQ) {
  CFX_ByteString null_string;
  EXPECT_TRUE(null_string == null_string);

  CFX_ByteString empty_string("");
  EXPECT_TRUE(empty_string == empty_string);
  EXPECT_TRUE(empty_string == null_string);
  EXPECT_TRUE(null_string == empty_string);

  CFX_ByteString deleted_string("hello");
  deleted_string.Delete(0, 5);
  EXPECT_TRUE(deleted_string == deleted_string);
  EXPECT_TRUE(deleted_string == null_string);
  EXPECT_TRUE(deleted_string == empty_string);
  EXPECT_TRUE(null_string == deleted_string);
  EXPECT_TRUE(empty_string == deleted_string);

  CFX_ByteString byte_string("hello");
  EXPECT_TRUE(byte_string == byte_string);
  EXPECT_FALSE(byte_string == null_string);
  EXPECT_FALSE(byte_string == empty_string);
  EXPECT_FALSE(byte_string == deleted_string);
  EXPECT_FALSE(null_string == byte_string);
  EXPECT_FALSE(empty_string == byte_string);
  EXPECT_FALSE(deleted_string == byte_string);

  CFX_ByteString byte_string_same1("hello");
  EXPECT_TRUE(byte_string == byte_string_same1);
  EXPECT_TRUE(byte_string_same1 == byte_string);

  CFX_ByteString byte_string_same2(byte_string);
  EXPECT_TRUE(byte_string == byte_string_same2);
  EXPECT_TRUE(byte_string_same2 == byte_string);

  CFX_ByteString byte_string1("he");
  CFX_ByteString byte_string2("hellp");
  CFX_ByteString byte_string3("hellod");
  EXPECT_FALSE(byte_string == byte_string1);
  EXPECT_FALSE(byte_string == byte_string2);
  EXPECT_FALSE(byte_string == byte_string3);
  EXPECT_FALSE(byte_string1 == byte_string);
  EXPECT_FALSE(byte_string2 == byte_string);
  EXPECT_FALSE(byte_string3 == byte_string);

  CFX_ByteStringC null_string_c;
  CFX_ByteStringC empty_string_c("");
  EXPECT_TRUE(null_string == null_string_c);
  EXPECT_TRUE(null_string == empty_string_c);
  EXPECT_TRUE(empty_string == null_string_c);
  EXPECT_TRUE(empty_string == empty_string_c);
  EXPECT_TRUE(deleted_string == null_string_c);
  EXPECT_TRUE(deleted_string == empty_string_c);
  EXPECT_TRUE(null_string_c == null_string);
  EXPECT_TRUE(empty_string_c == null_string);
  EXPECT_TRUE(null_string_c == empty_string);
  EXPECT_TRUE(empty_string_c == empty_string);
  EXPECT_TRUE(null_string_c == deleted_string);
  EXPECT_TRUE(empty_string_c == deleted_string);

  CFX_ByteStringC byte_string_c_same1("hello");
  EXPECT_TRUE(byte_string == byte_string_c_same1);
  EXPECT_TRUE(byte_string_c_same1 == byte_string);

  CFX_ByteStringC byte_string_c1("he");
  CFX_ByteStringC byte_string_c2("hellp");
  CFX_ByteStringC byte_string_c3("hellod");
  EXPECT_FALSE(byte_string == byte_string_c1);
  EXPECT_FALSE(byte_string == byte_string_c2);
  EXPECT_FALSE(byte_string == byte_string_c3);
  EXPECT_FALSE(byte_string_c1 == byte_string);
  EXPECT_FALSE(byte_string_c2 == byte_string);
  EXPECT_FALSE(byte_string_c3 == byte_string);

  const char* c_null_string = nullptr;
  const char* c_empty_string = "";
  EXPECT_TRUE(null_string == c_null_string);
  EXPECT_TRUE(null_string == c_empty_string);
  EXPECT_TRUE(empty_string == c_null_string);
  EXPECT_TRUE(empty_string == c_empty_string);
  EXPECT_TRUE(deleted_string == c_null_string);
  EXPECT_TRUE(deleted_string == c_empty_string);
  EXPECT_TRUE(c_null_string == null_string);
  EXPECT_TRUE(c_empty_string == null_string);
  EXPECT_TRUE(c_null_string == empty_string);
  EXPECT_TRUE(c_empty_string == empty_string);
  EXPECT_TRUE(c_null_string == deleted_string);
  EXPECT_TRUE(c_empty_string == deleted_string);

  const char* c_string_same1 = "hello";
  EXPECT_TRUE(byte_string == c_string_same1);
  EXPECT_TRUE(c_string_same1 == byte_string);

  const char* c_string1 = "he";
  const char* c_string2 = "hellp";
  const char* c_string3 = "hellod";
  EXPECT_FALSE(byte_string == c_string1);
  EXPECT_FALSE(byte_string == c_string2);
  EXPECT_FALSE(byte_string == c_string3);
  EXPECT_FALSE(c_string1 == byte_string);
  EXPECT_FALSE(c_string2 == byte_string);
  EXPECT_FALSE(c_string3 == byte_string);
}

TEST(fxcrt, ByteStringOperatorNE) {
  CFX_ByteString null_string;
  EXPECT_FALSE(null_string != null_string);

  CFX_ByteString empty_string("");
  EXPECT_FALSE(empty_string != empty_string);
  EXPECT_FALSE(empty_string != null_string);
  EXPECT_FALSE(null_string != empty_string);

  CFX_ByteString deleted_string("hello");
  deleted_string.Delete(0, 5);
  EXPECT_FALSE(deleted_string != deleted_string);
  EXPECT_FALSE(deleted_string != null_string);
  EXPECT_FALSE(deleted_string != empty_string);
  EXPECT_FALSE(deleted_string != deleted_string);
  EXPECT_FALSE(null_string != deleted_string);
  EXPECT_FALSE(empty_string != deleted_string);
  EXPECT_FALSE(deleted_string != deleted_string);

  CFX_ByteString byte_string("hello");
  EXPECT_FALSE(byte_string != byte_string);
  EXPECT_TRUE(byte_string != null_string);
  EXPECT_TRUE(byte_string != empty_string);
  EXPECT_TRUE(byte_string != deleted_string);
  EXPECT_TRUE(null_string != byte_string);
  EXPECT_TRUE(empty_string != byte_string);
  EXPECT_TRUE(deleted_string != byte_string);

  CFX_ByteString byte_string_same1("hello");
  EXPECT_FALSE(byte_string != byte_string_same1);
  EXPECT_FALSE(byte_string_same1 != byte_string);

  CFX_ByteString byte_string_same2(byte_string);
  EXPECT_FALSE(byte_string != byte_string_same2);
  EXPECT_FALSE(byte_string_same2 != byte_string);

  CFX_ByteString byte_string1("he");
  CFX_ByteString byte_string2("hellp");
  CFX_ByteString byte_string3("hellod");
  EXPECT_TRUE(byte_string != byte_string1);
  EXPECT_TRUE(byte_string != byte_string2);
  EXPECT_TRUE(byte_string != byte_string3);
  EXPECT_TRUE(byte_string1 != byte_string);
  EXPECT_TRUE(byte_string2 != byte_string);
  EXPECT_TRUE(byte_string3 != byte_string);

  CFX_ByteStringC null_string_c;
  CFX_ByteStringC empty_string_c("");
  EXPECT_FALSE(null_string != null_string_c);
  EXPECT_FALSE(null_string != empty_string_c);
  EXPECT_FALSE(empty_string != null_string_c);
  EXPECT_FALSE(empty_string != empty_string_c);
  EXPECT_FALSE(null_string_c != null_string);
  EXPECT_FALSE(empty_string_c != null_string);
  EXPECT_FALSE(null_string_c != empty_string);
  EXPECT_FALSE(empty_string_c != empty_string);

  CFX_ByteStringC byte_string_c_same1("hello");
  EXPECT_FALSE(byte_string != byte_string_c_same1);
  EXPECT_FALSE(byte_string_c_same1 != byte_string);

  CFX_ByteStringC byte_string_c1("he");
  CFX_ByteStringC byte_string_c2("hellp");
  CFX_ByteStringC byte_string_c3("hellod");
  EXPECT_TRUE(byte_string != byte_string_c1);
  EXPECT_TRUE(byte_string != byte_string_c2);
  EXPECT_TRUE(byte_string != byte_string_c3);
  EXPECT_TRUE(byte_string_c1 != byte_string);
  EXPECT_TRUE(byte_string_c2 != byte_string);
  EXPECT_TRUE(byte_string_c3 != byte_string);

  const char* c_null_string = nullptr;
  const char* c_empty_string = "";
  EXPECT_FALSE(null_string != c_null_string);
  EXPECT_FALSE(null_string != c_empty_string);
  EXPECT_FALSE(empty_string != c_null_string);
  EXPECT_FALSE(empty_string != c_empty_string);
  EXPECT_FALSE(deleted_string != c_null_string);
  EXPECT_FALSE(deleted_string != c_empty_string);
  EXPECT_FALSE(c_null_string != null_string);
  EXPECT_FALSE(c_empty_string != null_string);
  EXPECT_FALSE(c_null_string != empty_string);
  EXPECT_FALSE(c_empty_string != empty_string);
  EXPECT_FALSE(c_null_string != deleted_string);
  EXPECT_FALSE(c_empty_string != deleted_string);

  const char* c_string_same1 = "hello";
  EXPECT_FALSE(byte_string != c_string_same1);
  EXPECT_FALSE(c_string_same1 != byte_string);

  const char* c_string1 = "he";
  const char* c_string2 = "hellp";
  const char* c_string3 = "hellod";
  EXPECT_TRUE(byte_string != c_string1);
  EXPECT_TRUE(byte_string != c_string2);
  EXPECT_TRUE(byte_string != c_string3);
  EXPECT_TRUE(c_string1 != byte_string);
  EXPECT_TRUE(c_string2 != byte_string);
  EXPECT_TRUE(c_string3 != byte_string);
}

TEST(fxcrt, ByteStringCNull) {
  CFX_ByteStringC null_string;
  EXPECT_EQ(null_string.raw_str(), nullptr);
  EXPECT_EQ(null_string.GetLength(), 0);
  EXPECT_TRUE(null_string.IsEmpty());

  CFX_ByteStringC another_null_string;
  EXPECT_EQ(null_string, another_null_string);

  CFX_ByteStringC copied_null_string(null_string);
  EXPECT_EQ(copied_null_string.raw_str(), nullptr);
  EXPECT_EQ(copied_null_string.GetLength(), 0);
  EXPECT_TRUE(copied_null_string.IsEmpty());
  EXPECT_EQ(null_string, copied_null_string);

  CFX_ByteStringC empty_string("");  // Pointer to NUL, not NULL pointer.
  EXPECT_NE(empty_string.raw_str(), nullptr);
  EXPECT_EQ(empty_string.GetLength(), 0);
  EXPECT_TRUE(empty_string.IsEmpty());
  EXPECT_EQ(null_string, empty_string);

  CFX_ByteStringC assigned_null_string("initially not NULL");
  assigned_null_string = null_string;
  EXPECT_EQ(assigned_null_string.raw_str(), nullptr);
  EXPECT_EQ(assigned_null_string.GetLength(), 0);
  EXPECT_TRUE(assigned_null_string.IsEmpty());
  EXPECT_EQ(null_string, assigned_null_string);

  CFX_ByteStringC assigned_nullptr_string("initially not NULL");
  assigned_nullptr_string = (const FX_CHAR*)nullptr;
  EXPECT_EQ(assigned_nullptr_string.raw_str(), nullptr);
  EXPECT_EQ(assigned_nullptr_string.GetLength(), 0);
  EXPECT_TRUE(assigned_nullptr_string.IsEmpty());
  EXPECT_EQ(null_string, assigned_nullptr_string);

  CFX_ByteStringC non_null_string("a");
  EXPECT_NE(null_string, non_null_string);
}

TEST(fxcrt, ByteStringConcat) {
  CFX_ByteString fred;
  fred.Concat("FRED", 4);
  EXPECT_EQ("FRED", fred);

  fred.Concat("DY", 2);
  EXPECT_EQ("FREDDY", fred);

  fred.Delete(3, 3);
  EXPECT_EQ("FRE", fred);

  fred.Concat("D", 1);
  EXPECT_EQ("FRED", fred);

  CFX_ByteString copy = fred;
  fred.Concat("DY", 2);
  EXPECT_EQ("FREDDY", fred);
  EXPECT_EQ("FRED", copy);

  // Test invalid arguments.
  copy = fred;
  fred.Concat("freddy", -6);
  CFX_ByteString not_aliased("xxxxxx");
  EXPECT_EQ("FREDDY", fred);
  EXPECT_EQ("xxxxxx", not_aliased);
}

TEST(fxcrt, ByteStringRemove) {
  CFX_ByteString freed("FREED");
  freed.Remove('E');
  EXPECT_EQ("FRD", freed);
  freed.Remove('F');
  EXPECT_EQ("RD", freed);
  freed.Remove('D');
  EXPECT_EQ("R", freed);
  freed.Remove('X');
  EXPECT_EQ("R", freed);
  freed.Remove('R');
  EXPECT_EQ("", freed);

  CFX_ByteString empty;
  empty.Remove('X');
  EXPECT_EQ("", empty);
}

TEST(fxcrt, ByteStringReplace) {
  CFX_ByteString fred("FRED");
  fred.Replace("FR", "BL");
  EXPECT_EQ("BLED", fred);
  fred.Replace("D", "DDY");
  EXPECT_EQ("BLEDDY", fred);
  fred.Replace("LEDD", "");
  EXPECT_EQ("BY", fred);
  fred.Replace("X", "CLAMS");
  EXPECT_EQ("BY", fred);
  fred.Replace("BY", "HI");
  EXPECT_EQ("HI", fred);
  fred.Replace("", "CLAMS");
  EXPECT_EQ("HI", fred);
  fred.Replace("HI", "");
  EXPECT_EQ("", fred);
}

TEST(fxcrt, ByteStringInsert) {
  CFX_ByteString fred("FRED");
  fred.Insert(-1, 'X');
  EXPECT_EQ("XFRED", fred);
  fred.Insert(0, 'S');
  EXPECT_EQ("SXFRED", fred);
  fred.Insert(2, 'T');
  EXPECT_EQ("SXTFRED", fred);
  fred.Insert(5, 'U');
  EXPECT_EQ("SXTFRUED", fred);
  fred.Insert(8, 'V');
  EXPECT_EQ("SXTFRUEDV", fred);
  fred.Insert(12, 'P');
  EXPECT_EQ("SXTFRUEDVP", fred);
  {
    CFX_ByteString empty;
    empty.Insert(-1, 'X');
    EXPECT_EQ("X", empty);
  }
  {
    CFX_ByteString empty;
    empty.Insert(0, 'X');
    EXPECT_EQ("X", empty);
  }
  {
    CFX_ByteString empty;
    empty.Insert(5, 'X');
    EXPECT_EQ("X", empty);
  }
}

TEST(fxcrt, ByteStringDelete) {
  CFX_ByteString fred("FRED");
  fred.Delete(0, 2);
  EXPECT_EQ("ED", fred);
  fred.Delete(1);
  EXPECT_EQ("E", fred);
  fred.Delete(-1);
  EXPECT_EQ("", fred);
  fred.Delete(1);
  EXPECT_EQ("", fred);

  CFX_ByteString empty;
  empty.Delete(0);
  EXPECT_EQ("", empty);
  empty.Delete(-1);
  EXPECT_EQ("", empty);
  empty.Delete(1);
  EXPECT_EQ("", empty);
}

TEST(fxcrt, ByteStringMid) {
  CFX_ByteString fred("FRED");
  EXPECT_EQ("", fred.Mid(0, 0));
  EXPECT_EQ("", fred.Mid(3, 0));
  EXPECT_EQ("FRED", fred.Mid(0));
  EXPECT_EQ("RED", fred.Mid(1));
  EXPECT_EQ("ED", fred.Mid(2));
  EXPECT_EQ("D", fred.Mid(3));
  EXPECT_EQ("F", fred.Mid(0, 1));
  EXPECT_EQ("R", fred.Mid(1, 1));
  EXPECT_EQ("E", fred.Mid(2, 1));
  EXPECT_EQ("D", fred.Mid(3, 1));
  EXPECT_EQ("FR", fred.Mid(0, 2));
  EXPECT_EQ("FRED", fred.Mid(0, 4));
  EXPECT_EQ("FRED", fred.Mid(0, 10));

  EXPECT_EQ("FR", fred.Mid(-1, 2));
  EXPECT_EQ("RED", fred.Mid(1, 4));
  EXPECT_EQ("", fred.Mid(4, 1));

  CFX_ByteString empty;
  EXPECT_EQ("", empty.Mid(0, 0));
  EXPECT_EQ("", empty.Mid(0));
  EXPECT_EQ("", empty.Mid(1));
  EXPECT_EQ("", empty.Mid(-1));
}

TEST(fxcrt, ByteStringLeft) {
  CFX_ByteString fred("FRED");
  EXPECT_EQ("", fred.Left(0));
  EXPECT_EQ("F", fred.Left(1));
  EXPECT_EQ("FR", fred.Left(2));
  EXPECT_EQ("FRE", fred.Left(3));
  EXPECT_EQ("FRED", fred.Left(4));

  EXPECT_EQ("FRED", fred.Left(5));
  EXPECT_EQ("", fred.Left(-1));

  CFX_ByteString empty;
  EXPECT_EQ("", empty.Left(0));
  EXPECT_EQ("", empty.Left(1));
  EXPECT_EQ("", empty.Left(-1));
}

TEST(fxcrt, ByteStringRight) {
  CFX_ByteString fred("FRED");
  EXPECT_EQ("", fred.Right(0));
  EXPECT_EQ("D", fred.Right(1));
  EXPECT_EQ("ED", fred.Right(2));
  EXPECT_EQ("RED", fred.Right(3));
  EXPECT_EQ("FRED", fred.Right(4));

  EXPECT_EQ("FRED", fred.Right(5));
  EXPECT_EQ("", fred.Right(-1));

  CFX_ByteString empty;
  EXPECT_EQ("", empty.Right(0));
  EXPECT_EQ("", empty.Right(1));
  EXPECT_EQ("", empty.Right(-1));
}

TEST(fxcrt, ByteStringUpperLower) {
  CFX_ByteString fred("F-Re.42D");
  fred.MakeLower();
  EXPECT_EQ("f-re.42d", fred);
  fred.MakeUpper();
  EXPECT_EQ("F-RE.42D", fred);

  CFX_ByteString empty;
  empty.MakeLower();
  EXPECT_EQ("", empty);
  empty.MakeUpper();
  EXPECT_EQ("", empty);
}

TEST(fxcrt, ByteStringTrimRight) {
  CFX_ByteString fred("  FRED  ");
  fred.TrimRight();
  EXPECT_EQ("  FRED", fred);
  fred.TrimRight('E');
  EXPECT_EQ("  FRED", fred);
  fred.TrimRight('D');
  EXPECT_EQ("  FRE", fred);
  fred.TrimRight("ERP");
  EXPECT_EQ("  F", fred);

  CFX_ByteString blank("   ");
  blank.TrimRight("ERP");
  EXPECT_EQ("   ", blank);
  blank.TrimRight('E');
  EXPECT_EQ("   ", blank);
  blank.TrimRight();
  EXPECT_EQ("", blank);

  CFX_ByteString empty;
  empty.TrimRight("ERP");
  EXPECT_EQ("", empty);
  empty.TrimRight('E');
  EXPECT_EQ("", empty);
  empty.TrimRight();
  EXPECT_EQ("", empty);
}

TEST(fxcrt, ByteStringTrimLeft) {
  CFX_ByteString fred("  FRED  ");
  fred.TrimLeft();
  EXPECT_EQ("FRED  ", fred);
  fred.TrimLeft('E');
  EXPECT_EQ("FRED  ", fred);
  fred.TrimLeft('F');
  EXPECT_EQ("RED  ", fred);
  fred.TrimLeft("ERP");
  EXPECT_EQ("D  ", fred);

  CFX_ByteString blank("   ");
  blank.TrimLeft("ERP");
  EXPECT_EQ("   ", blank);
  blank.TrimLeft('E');
  EXPECT_EQ("   ", blank);
  blank.TrimLeft();
  EXPECT_EQ("", blank);

  CFX_ByteString empty;
  empty.TrimLeft("ERP");
  EXPECT_EQ("", empty);
  empty.TrimLeft('E');
  EXPECT_EQ("", empty);
  empty.TrimLeft();
  EXPECT_EQ("", empty);
}

TEST(fxcrt, ByteStringCNotNull) {
  CFX_ByteStringC string3("abc");
  CFX_ByteStringC string6("abcdef");
  CFX_ByteStringC alternate_string3("abcdef", 3);
  CFX_ByteStringC embedded_nul_string7("abc\0def", 7);
  CFX_ByteStringC illegal_string7("abcdef", 7);

  EXPECT_EQ(3, string3.GetLength());
  EXPECT_EQ(6, string6.GetLength());
  EXPECT_EQ(3, alternate_string3.GetLength());
  EXPECT_EQ(7, embedded_nul_string7.GetLength());
  EXPECT_EQ(7, illegal_string7.GetLength());

  EXPECT_NE(string3, string6);
  EXPECT_EQ(string3, alternate_string3);
  EXPECT_NE(string3, embedded_nul_string7);
  EXPECT_NE(string3, illegal_string7);
  EXPECT_NE(string6, alternate_string3);
  EXPECT_NE(string6, embedded_nul_string7);
  EXPECT_NE(string6, illegal_string7);
  EXPECT_NE(alternate_string3, embedded_nul_string7);
  EXPECT_NE(alternate_string3, illegal_string7);
  EXPECT_NE(embedded_nul_string7, illegal_string7);

  CFX_ByteStringC copied_string3(string3);
  CFX_ByteStringC copied_alternate_string3(alternate_string3);
  CFX_ByteStringC copied_embedded_nul_string7(embedded_nul_string7);

  EXPECT_EQ(string3, copied_string3);
  EXPECT_EQ(alternate_string3, copied_alternate_string3);
  EXPECT_EQ(embedded_nul_string7, copied_embedded_nul_string7);

  CFX_ByteStringC assigned_string3("intially something else");
  CFX_ByteStringC assigned_alternate_string3("initally something else");
  CFX_ByteStringC assigned_ptr_string3("initially something else");
  CFX_ByteStringC assigned_embedded_nul_string7("initially something else");

  assigned_string3 = string3;
  assigned_alternate_string3 = alternate_string3;
  assigned_ptr_string3 = "abc";
  assigned_embedded_nul_string7 = embedded_nul_string7;
  EXPECT_EQ(string3, assigned_string3);
  EXPECT_EQ(alternate_string3, assigned_alternate_string3);
  EXPECT_EQ(alternate_string3, assigned_ptr_string3);
  EXPECT_EQ(embedded_nul_string7, assigned_embedded_nul_string7);
}

TEST(fxcrt, ByteStringCFromChar) {
  CFX_ByteStringC null_string;
  CFX_ByteStringC lower_a_string("a");

  // Must have lvalues that outlive the corresponding ByteStringC.
  char nul = '\0';
  char lower_a = 'a';
  CFX_ByteStringC nul_string_from_char(nul);
  CFX_ByteStringC lower_a_string_from_char(lower_a);

  // Pointer to nul, not NULL ptr, hence length 1 ...
  EXPECT_EQ(1, nul_string_from_char.GetLength());
  EXPECT_NE(null_string, nul_string_from_char);

  EXPECT_EQ(1, lower_a_string_from_char.GetLength());
  EXPECT_EQ(lower_a_string, lower_a_string_from_char);
  EXPECT_NE(nul_string_from_char, lower_a_string_from_char);

  CFX_ByteStringC longer_string("ab");
  EXPECT_NE(longer_string, lower_a_string_from_char);
}

TEST(fxcrt, ByteStringCGetID) {
  CFX_ByteStringC null_string;
  EXPECT_EQ(0u, null_string.GetID());
  EXPECT_EQ(0u, null_string.GetID(1));
  EXPECT_EQ(0u, null_string.GetID(-1));
  EXPECT_EQ(0u, null_string.GetID(-1000000));

  CFX_ByteStringC empty_string("");
  EXPECT_EQ(0u, empty_string.GetID());
  EXPECT_EQ(0u, empty_string.GetID(1));
  EXPECT_EQ(0u, empty_string.GetID(-1));
  EXPECT_EQ(0u, empty_string.GetID(-1000000));

  CFX_ByteStringC short_string("ab");
  EXPECT_EQ(FXBSTR_ID('a', 'b', 0, 0), short_string.GetID());
  EXPECT_EQ(FXBSTR_ID('b', 0, 0, 0), short_string.GetID(1));
  EXPECT_EQ(0u, short_string.GetID(2));
  EXPECT_EQ(0u, short_string.GetID(-1));
  EXPECT_EQ(0u, short_string.GetID(-1000000));

  CFX_ByteStringC longer_string("abcdef");
  EXPECT_EQ(FXBSTR_ID('a', 'b', 'c', 'd'), longer_string.GetID());
  EXPECT_EQ(FXBSTR_ID('b', 'c', 'd', 'e'), longer_string.GetID(1));
  EXPECT_EQ(FXBSTR_ID('c', 'd', 'e', 'f'), longer_string.GetID(2));
  EXPECT_EQ(FXBSTR_ID('d', 'e', 'f', 0), longer_string.GetID(3));
  EXPECT_EQ(FXBSTR_ID('e', 'f', 0, 0), longer_string.GetID(4));
  EXPECT_EQ(FXBSTR_ID('f', 0, 0, 0), longer_string.GetID(5));
  EXPECT_EQ(0u, longer_string.GetID(6));
  EXPECT_EQ(0u, longer_string.GetID(-1));
  EXPECT_EQ(0u, longer_string.GetID(-1000000));
}

TEST(fxcrt, ByteStringCMid) {
  CFX_ByteStringC null_string;
  EXPECT_EQ(null_string, null_string.Mid(0, 1));
  EXPECT_EQ(null_string, null_string.Mid(1, 1));

  CFX_ByteStringC empty_string("");
  EXPECT_EQ(empty_string, empty_string.Mid(0, 1));
  EXPECT_EQ(empty_string, empty_string.Mid(1, 1));

  CFX_ByteStringC single_character("a");
  EXPECT_EQ(empty_string, single_character.Mid(0, 0));
  EXPECT_EQ(single_character, single_character.Mid(0, 1));
  EXPECT_EQ(empty_string, single_character.Mid(1, 0));
  EXPECT_EQ(empty_string, single_character.Mid(1, 1));

  CFX_ByteStringC longer_string("abcdef");
  EXPECT_EQ(longer_string, longer_string.Mid(0, 6));
  EXPECT_EQ(longer_string, longer_string.Mid(0, 187));
  EXPECT_EQ(longer_string, longer_string.Mid(-42, 6));
  EXPECT_EQ(longer_string, longer_string.Mid(-42, 187));

  CFX_ByteStringC leading_substring("ab");
  EXPECT_EQ(leading_substring, longer_string.Mid(0, 2));
  EXPECT_EQ(leading_substring, longer_string.Mid(-1, 2));

  CFX_ByteStringC middle_substring("bcde");
  EXPECT_EQ(middle_substring, longer_string.Mid(1, 4));

  CFX_ByteStringC trailing_substring("ef");
  EXPECT_EQ(trailing_substring, longer_string.Mid(4, 2));
  EXPECT_EQ(trailing_substring, longer_string.Mid(4, 3));
}

TEST(fxcrt, ByteStringCGetAt) {
  CFX_ByteString short_string("a");
  CFX_ByteString longer_string("abc");
  CFX_ByteString embedded_nul_string("ab\0c", 4);

  EXPECT_EQ('a', short_string.GetAt(0));
  EXPECT_EQ('c', longer_string.GetAt(2));
  EXPECT_EQ('b', embedded_nul_string.GetAt(1));
  EXPECT_EQ('\0', embedded_nul_string.GetAt(2));
  EXPECT_EQ('c', embedded_nul_string.GetAt(3));
}

TEST(fxcrt, ByteStringCOperatorSubscript) {
  // CFX_ByteStringC includes the NUL terminator for non-empty strings.
  CFX_ByteStringC abc("abc");
  EXPECT_EQ('a', abc[0]);
  EXPECT_EQ('b', abc[1]);
  EXPECT_EQ('c', abc[2]);
  EXPECT_EQ(0, abc[3]);
}

TEST(fxcrt, ByteStringCOperatorLT) {
  CFX_ByteStringC empty;
  CFX_ByteStringC a("a");
  CFX_ByteStringC abc("abc");
  CFX_ByteStringC def("def");

  EXPECT_FALSE(empty < empty);
  EXPECT_FALSE(a < a);
  EXPECT_FALSE(abc < abc);
  EXPECT_FALSE(def < def);

  EXPECT_TRUE(empty < a);
  EXPECT_FALSE(a < empty);

  EXPECT_TRUE(empty < abc);
  EXPECT_FALSE(abc < empty);

  EXPECT_TRUE(empty < def);
  EXPECT_FALSE(def < empty);

  EXPECT_TRUE(a < abc);
  EXPECT_FALSE(abc < a);

  EXPECT_TRUE(a < def);
  EXPECT_FALSE(def < a);

  EXPECT_TRUE(abc < def);
  EXPECT_FALSE(def < abc);
}

TEST(fxcrt, ByteStringCOperatorEQ) {
  CFX_ByteStringC byte_string_c("hello");
  EXPECT_TRUE(byte_string_c == byte_string_c);

  CFX_ByteStringC byte_string_c_same1("hello");
  EXPECT_TRUE(byte_string_c == byte_string_c_same1);
  EXPECT_TRUE(byte_string_c_same1 == byte_string_c);

  CFX_ByteStringC byte_string_c_same2(byte_string_c);
  EXPECT_TRUE(byte_string_c == byte_string_c_same2);
  EXPECT_TRUE(byte_string_c_same2 == byte_string_c);

  CFX_ByteStringC byte_string_c1("he");
  CFX_ByteStringC byte_string_c2("hellp");
  CFX_ByteStringC byte_string_c3("hellod");
  EXPECT_FALSE(byte_string_c == byte_string_c1);
  EXPECT_FALSE(byte_string_c == byte_string_c2);
  EXPECT_FALSE(byte_string_c == byte_string_c3);
  EXPECT_FALSE(byte_string_c1 == byte_string_c);
  EXPECT_FALSE(byte_string_c2 == byte_string_c);
  EXPECT_FALSE(byte_string_c3 == byte_string_c);

  CFX_ByteString byte_string_same1("hello");
  EXPECT_TRUE(byte_string_c == byte_string_same1);
  EXPECT_TRUE(byte_string_same1 == byte_string_c);

  CFX_ByteString byte_string1("he");
  CFX_ByteString byte_string2("hellp");
  CFX_ByteString byte_string3("hellod");
  EXPECT_FALSE(byte_string_c == byte_string1);
  EXPECT_FALSE(byte_string_c == byte_string2);
  EXPECT_FALSE(byte_string_c == byte_string3);
  EXPECT_FALSE(byte_string1 == byte_string_c);
  EXPECT_FALSE(byte_string2 == byte_string_c);
  EXPECT_FALSE(byte_string3 == byte_string_c);

  const char* c_string_same1 = "hello";
  EXPECT_TRUE(byte_string_c == c_string_same1);
  EXPECT_TRUE(c_string_same1 == byte_string_c);

  const char* c_string1 = "he";
  const char* c_string2 = "hellp";
  const char* c_string3 = "hellod";
  EXPECT_FALSE(byte_string_c == c_string1);
  EXPECT_FALSE(byte_string_c == c_string2);
  EXPECT_FALSE(byte_string_c == c_string3);

  EXPECT_FALSE(c_string1 == byte_string_c);
  EXPECT_FALSE(c_string2 == byte_string_c);
  EXPECT_FALSE(c_string3 == byte_string_c);
}

TEST(fxcrt, ByteStringCOperatorNE) {
  CFX_ByteStringC byte_string_c("hello");
  EXPECT_FALSE(byte_string_c != byte_string_c);

  CFX_ByteStringC byte_string_c_same1("hello");
  EXPECT_FALSE(byte_string_c != byte_string_c_same1);
  EXPECT_FALSE(byte_string_c_same1 != byte_string_c);

  CFX_ByteStringC byte_string_c_same2(byte_string_c);
  EXPECT_FALSE(byte_string_c != byte_string_c_same2);
  EXPECT_FALSE(byte_string_c_same2 != byte_string_c);

  CFX_ByteStringC byte_string_c1("he");
  CFX_ByteStringC byte_string_c2("hellp");
  CFX_ByteStringC byte_string_c3("hellod");
  EXPECT_TRUE(byte_string_c != byte_string_c1);
  EXPECT_TRUE(byte_string_c != byte_string_c2);
  EXPECT_TRUE(byte_string_c != byte_string_c3);
  EXPECT_TRUE(byte_string_c1 != byte_string_c);
  EXPECT_TRUE(byte_string_c2 != byte_string_c);
  EXPECT_TRUE(byte_string_c3 != byte_string_c);

  CFX_ByteString byte_string_same1("hello");
  EXPECT_FALSE(byte_string_c != byte_string_same1);
  EXPECT_FALSE(byte_string_same1 != byte_string_c);

  CFX_ByteString byte_string1("he");
  CFX_ByteString byte_string2("hellp");
  CFX_ByteString byte_string3("hellod");
  EXPECT_TRUE(byte_string_c != byte_string1);
  EXPECT_TRUE(byte_string_c != byte_string2);
  EXPECT_TRUE(byte_string_c != byte_string3);
  EXPECT_TRUE(byte_string1 != byte_string_c);
  EXPECT_TRUE(byte_string2 != byte_string_c);
  EXPECT_TRUE(byte_string3 != byte_string_c);

  const char* c_string_same1 = "hello";
  EXPECT_FALSE(byte_string_c != c_string_same1);
  EXPECT_FALSE(c_string_same1 != byte_string_c);

  const char* c_string1 = "he";
  const char* c_string2 = "hellp";
  const char* c_string3 = "hellod";
  EXPECT_TRUE(byte_string_c != c_string1);
  EXPECT_TRUE(byte_string_c != c_string2);
  EXPECT_TRUE(byte_string_c != c_string3);

  EXPECT_TRUE(c_string1 != byte_string_c);
  EXPECT_TRUE(c_string2 != byte_string_c);
  EXPECT_TRUE(c_string3 != byte_string_c);
}

TEST(fxcrt, ByteStringFormatWidth) {
  {
    CFX_ByteString str;
    str.Format("%5d", 1);
    EXPECT_EQ("    1", str);
  }

  {
    CFX_ByteString str;
    str.Format("%d", 1);
    EXPECT_EQ("1", str);
  }

  {
    CFX_ByteString str;
    str.Format("%*d", 5, 1);
    EXPECT_EQ("    1", str);
  }

  {
    CFX_ByteString str;
    str.Format("%-1d", 1);
    EXPECT_EQ("1", str);
  }

  {
    CFX_ByteString str;
    str.Format("%0d", 1);
    EXPECT_EQ("1", str);
  }

  {
    CFX_ByteString str;
    str.Format("%1048576d", 1);
    EXPECT_EQ("Bad width", str);
  }
}

TEST(fxcrt, ByteStringFormatPrecision) {
  {
    CFX_ByteString str;
    str.Format("%.2f", 1.12345);
    EXPECT_EQ("1.12", str);
  }

  {
    CFX_ByteString str;
    str.Format("%.*f", 3, 1.12345);
    EXPECT_EQ("1.123", str);
  }

  {
    CFX_ByteString str;
    str.Format("%f", 1.12345);
    EXPECT_EQ("1.123450", str);
  }

  {
    CFX_ByteString str;
    str.Format("%-1f", 1.12345);
    EXPECT_EQ("1.123450", str);
  }

  {
    CFX_ByteString str;
    str.Format("%0f", 1.12345);
    EXPECT_EQ("1.123450", str);
  }

  {
    CFX_ByteString str;
    str.Format("%.1048576f", 1.2);
    EXPECT_EQ("Bad precision", str);
  }
}

TEST(fxcrt, EmptyByteString) {
  CFX_ByteString empty_str;
  EXPECT_TRUE(empty_str.IsEmpty());
  EXPECT_EQ(0, empty_str.GetLength());
  const FX_CHAR* cstr = empty_str.c_str();
  EXPECT_EQ(0, FXSYS_strlen(cstr));
}

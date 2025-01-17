// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/fpdf_parser/cpdf_boolean.h"
#include "core/fpdfapi/fpdf_parser/cpdf_null.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_name.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_number.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_reference.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_stream.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_string.h"

#include <memory>
#include <string>
#include <vector>

#include "core/fpdfapi/fpdf_parser/include/cpdf_indirect_object_holder.h"
#include "core/fxcrt/include/fx_basic.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

using ScopedArray = std::unique_ptr<CPDF_Array, ReleaseDeleter<CPDF_Array>>;

void TestArrayAccessors(const CPDF_Array* arr,
                        size_t index,
                        const char* str_val,
                        const char* const_str_val,
                        int int_val,
                        float float_val,
                        CPDF_Array* arr_val,
                        CPDF_Dictionary* dict_val,
                        CPDF_Stream* stream_val) {
  EXPECT_STREQ(str_val, arr->GetStringAt(index).c_str());
  EXPECT_STREQ(const_str_val, arr->GetConstStringAt(index).c_str());
  EXPECT_EQ(int_val, arr->GetIntegerAt(index));
  EXPECT_EQ(float_val, arr->GetNumberAt(index));
  EXPECT_EQ(float_val, arr->GetFloatAt(index));
  EXPECT_EQ(arr_val, arr->GetArrayAt(index));
  EXPECT_EQ(dict_val, arr->GetDictAt(index));
  EXPECT_EQ(stream_val, arr->GetStreamAt(index));
}

}  // namespace

class PDFObjectsTest : public testing::Test {
 public:
  void SetUp() override {
    // Initialize different kinds of objects.
    // Boolean objects.
    CPDF_Boolean* boolean_false_obj = new CPDF_Boolean(false);
    CPDF_Boolean* boolean_true_obj = new CPDF_Boolean(true);
    // Number objects.
    CPDF_Number* number_int_obj = new CPDF_Number(1245);
    CPDF_Number* number_float_obj = new CPDF_Number(9.00345f);
    // String objects.
    CPDF_String* str_reg_obj = new CPDF_String(L"A simple test");
    CPDF_String* str_spec_obj = new CPDF_String(L"\t\n");
    // Name object.
    CPDF_Name* name_obj = new CPDF_Name("space");
    // Array object.
    m_ArrayObj = new CPDF_Array;
    m_ArrayObj->InsertAt(0, new CPDF_Number(8902));
    m_ArrayObj->InsertAt(1, new CPDF_Name("address"));
    // Dictionary object.
    m_DictObj = new CPDF_Dictionary;
    m_DictObj->SetAt("bool", new CPDF_Boolean(false));
    m_DictObj->SetAt("num", new CPDF_Number(0.23f));
    // Stream object.
    const char content[] = "abcdefghijklmnopqrstuvwxyz";
    size_t buf_len = FX_ArraySize(content);
    uint8_t* buf = reinterpret_cast<uint8_t*>(malloc(buf_len));
    memcpy(buf, content, buf_len);
    m_StreamDictObj = new CPDF_Dictionary;
    m_StreamDictObj->SetAt("key1", new CPDF_String(L" test dict"));
    m_StreamDictObj->SetAt("key2", new CPDF_Number(-1));
    CPDF_Stream* stream_obj = new CPDF_Stream(buf, buf_len, m_StreamDictObj);
    // Null Object.
    CPDF_Null* null_obj = new CPDF_Null;
    // All direct objects.
    CPDF_Object* objs[] = {boolean_false_obj, boolean_true_obj, number_int_obj,
                           number_float_obj,  str_reg_obj,      str_spec_obj,
                           name_obj,          m_ArrayObj,       m_DictObj,
                           stream_obj,        null_obj};
    m_DirectObjTypes = {
        CPDF_Object::BOOLEAN, CPDF_Object::BOOLEAN, CPDF_Object::NUMBER,
        CPDF_Object::NUMBER,  CPDF_Object::STRING,  CPDF_Object::STRING,
        CPDF_Object::NAME,    CPDF_Object::ARRAY,   CPDF_Object::DICTIONARY,
        CPDF_Object::STREAM,  CPDF_Object::NULLOBJ};
    for (size_t i = 0; i < FX_ArraySize(objs); ++i)
      m_DirectObjs.emplace_back(objs[i]);

    // Indirect references to indirect objects.
    m_ObjHolder.reset(new CPDF_IndirectObjectHolder(nullptr));
    m_IndirectObjs = {boolean_true_obj, number_int_obj, str_spec_obj, name_obj,
                      m_ArrayObj,       m_DictObj,      stream_obj};
    for (size_t i = 0; i < m_IndirectObjs.size(); ++i) {
      m_ObjHolder->AddIndirectObject(m_IndirectObjs[i]);
      m_RefObjs.emplace_back(new CPDF_Reference(
          m_ObjHolder.get(), m_IndirectObjs[i]->GetObjNum()));
    }
  }

  bool Equal(CPDF_Object* obj1, CPDF_Object* obj2) {
    if (obj1 == obj2)
      return true;
    if (!obj1 || !obj2 || obj1->GetType() != obj2->GetType())
      return false;
    switch (obj1->GetType()) {
      case CPDF_Object::BOOLEAN:
        return obj1->GetInteger() == obj2->GetInteger();
      case CPDF_Object::NUMBER:
        return obj1->AsNumber()->IsInteger() == obj2->AsNumber()->IsInteger() &&
               obj1->GetInteger() == obj2->GetInteger();
      case CPDF_Object::STRING:
      case CPDF_Object::NAME:
        return obj1->GetString() == obj2->GetString();
      case CPDF_Object::ARRAY: {
        const CPDF_Array* array1 = obj1->AsArray();
        const CPDF_Array* array2 = obj2->AsArray();
        if (array1->GetCount() != array2->GetCount())
          return false;
        for (size_t i = 0; i < array1->GetCount(); ++i) {
          if (!Equal(array1->GetObjectAt(i), array2->GetObjectAt(i)))
            return false;
        }
        return true;
      }
      case CPDF_Object::DICTIONARY: {
        const CPDF_Dictionary* dict1 = obj1->AsDictionary();
        const CPDF_Dictionary* dict2 = obj2->AsDictionary();
        if (dict1->GetCount() != dict2->GetCount())
          return false;
        for (CPDF_Dictionary::const_iterator it = dict1->begin();
             it != dict1->end(); ++it) {
          if (!Equal(it->second, dict2->GetObjectBy(it->first.AsByteStringC())))
            return false;
        }
        return true;
      }
      case CPDF_Object::NULLOBJ:
        return true;
      case CPDF_Object::STREAM: {
        const CPDF_Stream* stream1 = obj1->AsStream();
        const CPDF_Stream* stream2 = obj2->AsStream();
        if (!stream1->GetDict() && !stream2->GetDict())
          return true;
        // Compare dictionaries.
        if (!Equal(stream1->GetDict(), stream2->GetDict()))
          return false;
        // Compare sizes.
        if (stream1->GetRawSize() != stream2->GetRawSize())
          return false;
        // Compare contents.
        // Since this function is used for testing Clone(), only memory based
        // streams need to be handled.
        if (!stream1->IsMemoryBased() || !stream2->IsMemoryBased())
          return false;
        return FXSYS_memcmp(stream1->GetRawData(), stream2->GetRawData(),
                            stream1->GetRawSize()) == 0;
      }
      case CPDF_Object::REFERENCE:
        return obj1->AsReference()->GetRefObjNum() ==
               obj2->AsReference()->GetRefObjNum();
    }
    return false;
  }

 protected:
  using ScopedObj = std::unique_ptr<CPDF_Object, ReleaseDeleter<CPDF_Object>>;

  // m_ObjHolder needs to be declared first and destructed last since it also
  // refers to some objects in m_DirectObjs.
  std::unique_ptr<CPDF_IndirectObjectHolder> m_ObjHolder;
  std::vector<ScopedObj> m_DirectObjs;
  std::vector<int> m_DirectObjTypes;
  std::vector<ScopedObj> m_RefObjs;
  CPDF_Dictionary* m_DictObj;
  CPDF_Dictionary* m_StreamDictObj;
  CPDF_Array* m_ArrayObj;
  std::vector<CPDF_Object*> m_IndirectObjs;
};

TEST_F(PDFObjectsTest, GetString) {
  const char* direct_obj_results[] = {
      "false", "true", "1245", "9.00345", "A simple test", "\t\n", "space",
      "",      "",     "",     ""};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_STREQ(direct_obj_results[i], m_DirectObjs[i]->GetString().c_str());

  // Check indirect references.
  const char* indirect_obj_results[] = {"true", "1245", "\t\n", "space",
                                        "",     "",     ""};
  for (size_t i = 0; i < m_RefObjs.size(); ++i) {
    EXPECT_STREQ(indirect_obj_results[i], m_RefObjs[i]->GetString().c_str());
  }
}

TEST_F(PDFObjectsTest, GetConstString) {
  const char* direct_obj_results[] = {
      nullptr, nullptr, nullptr, nullptr, "A simple test", "\t\n",
      "space", nullptr, nullptr, nullptr, nullptr};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i) {
    if (!direct_obj_results[i]) {
      EXPECT_EQ(direct_obj_results[i],
                m_DirectObjs[i]->GetConstString().c_str());
    } else {
      EXPECT_STREQ(direct_obj_results[i],
                   m_DirectObjs[i]->GetConstString().c_str());
    }
  }
  // Check indirect references.
  const char* indirect_obj_results[] = {nullptr, nullptr, "\t\n", "space",
                                        nullptr, nullptr, nullptr};
  for (size_t i = 0; i < m_RefObjs.size(); ++i) {
    if (!indirect_obj_results[i]) {
      EXPECT_EQ(nullptr, m_RefObjs[i]->GetConstString().c_str());
    } else {
      EXPECT_STREQ(indirect_obj_results[i],
                   m_RefObjs[i]->GetConstString().c_str());
    }
  }
}

TEST_F(PDFObjectsTest, GetUnicodeText) {
  const wchar_t* direct_obj_results[] = {
      L"",     L"",      L"", L"", L"A simple test",
      L"\t\n", L"space", L"", L"", L"abcdefghijklmnopqrstuvwxyz",
      L""};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_STREQ(direct_obj_results[i],
                 m_DirectObjs[i]->GetUnicodeText().c_str());

  // Check indirect references.
  for (const auto& it : m_RefObjs)
    EXPECT_STREQ(L"", it->GetUnicodeText().c_str());
}

TEST_F(PDFObjectsTest, GetNumber) {
  const FX_FLOAT direct_obj_results[] = {0, 0, 1245, 9.00345f, 0, 0,
                                         0, 0, 0,    0,        0};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(direct_obj_results[i], m_DirectObjs[i]->GetNumber());

  // Check indirect references.
  const FX_FLOAT indirect_obj_results[] = {0, 1245, 0, 0, 0, 0, 0};
  for (size_t i = 0; i < m_RefObjs.size(); ++i)
    EXPECT_EQ(indirect_obj_results[i], m_RefObjs[i]->GetNumber());
}

TEST_F(PDFObjectsTest, GetInteger) {
  const int direct_obj_results[] = {0, 1, 1245, 9, 0, 0, 0, 0, 0, 0, 0};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(direct_obj_results[i], m_DirectObjs[i]->GetInteger());

  // Check indirect references.
  const int indirect_obj_results[] = {1, 1245, 0, 0, 0, 0, 0};
  for (size_t i = 0; i < m_RefObjs.size(); ++i)
    EXPECT_EQ(indirect_obj_results[i], m_RefObjs[i]->GetInteger());
}

TEST_F(PDFObjectsTest, GetDict) {
  const CPDF_Dictionary* direct_obj_results[] = {
      nullptr, nullptr, nullptr,   nullptr,         nullptr, nullptr,
      nullptr, nullptr, m_DictObj, m_StreamDictObj, nullptr};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(direct_obj_results[i], m_DirectObjs[i]->GetDict());

  // Check indirect references.
  const CPDF_Dictionary* indirect_obj_results[] = {
      nullptr, nullptr, nullptr, nullptr, nullptr, m_DictObj, m_StreamDictObj};
  for (size_t i = 0; i < m_RefObjs.size(); ++i)
    EXPECT_EQ(indirect_obj_results[i], m_RefObjs[i]->GetDict());
}

TEST_F(PDFObjectsTest, GetArray) {
  const CPDF_Array* direct_obj_results[] = {
      nullptr, nullptr,    nullptr, nullptr, nullptr, nullptr,
      nullptr, m_ArrayObj, nullptr, nullptr, nullptr};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(direct_obj_results[i], m_DirectObjs[i]->GetArray());

  // Check indirect references.
  for (const auto& it : m_RefObjs)
    EXPECT_EQ(nullptr, it->GetArray());
}

TEST_F(PDFObjectsTest, Clone) {
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i) {
    ScopedObj obj(m_DirectObjs[i]->Clone());
    EXPECT_TRUE(Equal(m_DirectObjs[i].get(), obj.get()));
  }

  // Check indirect references.
  for (const auto& it : m_RefObjs) {
    ScopedObj obj(it->Clone());
    EXPECT_TRUE(Equal(it.get(), obj.get()));
  }
}

TEST_F(PDFObjectsTest, GetType) {
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(m_DirectObjTypes[i], m_DirectObjs[i]->GetType());

  // Check indirect references.
  for (const auto& it : m_RefObjs)
    EXPECT_EQ(CPDF_Object::REFERENCE, it->GetType());
}

TEST_F(PDFObjectsTest, GetDirect) {
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->GetDirect());

  // Check indirect references.
  for (size_t i = 0; i < m_RefObjs.size(); ++i)
    EXPECT_EQ(m_IndirectObjs[i], m_RefObjs[i]->GetDirect());
}

TEST_F(PDFObjectsTest, SetString) {
  // Check for direct objects.
  const char* const set_values[] = {"true",    "fake", "3.125f", "097",
                                    "changed", "",     "NewName"};
  const char* expected[] = {"true",    "false", "3.125",  "97",
                            "changed", "",      "NewName"};
  for (size_t i = 0; i < FX_ArraySize(set_values); ++i) {
    m_DirectObjs[i]->SetString(set_values[i]);
    EXPECT_STREQ(expected[i], m_DirectObjs[i]->GetString().c_str());
  }
}

TEST_F(PDFObjectsTest, IsTypeAndAsType) {
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i) {
    if (m_DirectObjTypes[i] == CPDF_Object::ARRAY) {
      EXPECT_TRUE(m_DirectObjs[i]->IsArray());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsArray());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsArray());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsArray());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::BOOLEAN) {
      EXPECT_TRUE(m_DirectObjs[i]->IsBoolean());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsBoolean());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsBoolean());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsBoolean());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::NAME) {
      EXPECT_TRUE(m_DirectObjs[i]->IsName());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsName());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsName());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsName());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::NUMBER) {
      EXPECT_TRUE(m_DirectObjs[i]->IsNumber());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsNumber());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsNumber());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsNumber());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::STRING) {
      EXPECT_TRUE(m_DirectObjs[i]->IsString());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsString());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsString());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsString());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::DICTIONARY) {
      EXPECT_TRUE(m_DirectObjs[i]->IsDictionary());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsDictionary());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsDictionary());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsDictionary());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::STREAM) {
      EXPECT_TRUE(m_DirectObjs[i]->IsStream());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsStream());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsStream());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsStream());
    }

    EXPECT_FALSE(m_DirectObjs[i]->IsReference());
    EXPECT_EQ(nullptr, m_DirectObjs[i]->AsReference());
  }
  // Check indirect references.
  for (size_t i = 0; i < m_RefObjs.size(); ++i) {
    EXPECT_TRUE(m_RefObjs[i]->IsReference());
    EXPECT_EQ(m_RefObjs[i].get(), m_RefObjs[i]->AsReference());
  }
}

TEST(PDFArrayTest, GetMatrix) {
  float elems[][6] = {{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                      {1, 2, 3, 4, 5, 6},
                      {2.3f, 4.05f, 3, -2, -3, 0.0f},
                      {0.05f, 0.1f, 0.56f, 0.67f, 1.34f, 99.9f}};
  for (size_t i = 0; i < FX_ArraySize(elems); ++i) {
    ScopedArray arr(new CPDF_Array);
    CFX_Matrix matrix(elems[i][0], elems[i][1], elems[i][2], elems[i][3],
                      elems[i][4], elems[i][5]);
    for (size_t j = 0; j < 6; ++j)
      arr->AddNumber(elems[i][j]);
    CFX_Matrix arr_matrix = arr->GetMatrix();
    EXPECT_EQ(matrix.GetA(), arr_matrix.GetA());
    EXPECT_EQ(matrix.GetB(), arr_matrix.GetB());
    EXPECT_EQ(matrix.GetC(), arr_matrix.GetC());
    EXPECT_EQ(matrix.GetD(), arr_matrix.GetD());
    EXPECT_EQ(matrix.GetE(), arr_matrix.GetE());
    EXPECT_EQ(matrix.GetF(), arr_matrix.GetF());
  }
}

TEST(PDFArrayTest, GetRect) {
  float elems[][4] = {{0.0f, 0.0f, 0.0f, 0.0f},
                      {1, 2, 5, 6},
                      {2.3f, 4.05f, -3, 0.0f},
                      {0.05f, 0.1f, 1.34f, 99.9f}};
  for (size_t i = 0; i < FX_ArraySize(elems); ++i) {
    ScopedArray arr(new CPDF_Array);
    CFX_FloatRect rect(elems[i]);
    for (size_t j = 0; j < 4; ++j)
      arr->AddNumber(elems[i][j]);
    CFX_FloatRect arr_rect = arr->GetRect();
    EXPECT_EQ(rect.left, arr_rect.left);
    EXPECT_EQ(rect.right, arr_rect.right);
    EXPECT_EQ(rect.bottom, arr_rect.bottom);
    EXPECT_EQ(rect.top, arr_rect.top);
  }
}

TEST(PDFArrayTest, GetTypeAt) {
  {
    // Boolean array.
    bool vals[] = {true, false, false, true, true};
    ScopedArray arr(new CPDF_Array);
    for (size_t i = 0; i < FX_ArraySize(vals); ++i)
      arr->InsertAt(i, new CPDF_Boolean(vals[i]));
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      TestArrayAccessors(arr.get(), i,                // Array and index.
                         vals[i] ? "true" : "false",  // String value.
                         nullptr,                     // Const string value.
                         vals[i] ? 1 : 0,             // Integer value.
                         0,                           // Float value.
                         nullptr,                     // Array value.
                         nullptr,                     // Dictionary value.
                         nullptr);                    // Stream value.
    }
  }
  {
    // Integer array.
    int vals[] = {10, 0, -345, 2089345456, -1000000000, 567, 93658767};
    ScopedArray arr(new CPDF_Array);
    for (size_t i = 0; i < FX_ArraySize(vals); ++i)
      arr->InsertAt(i, new CPDF_Number(vals[i]));
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      char buf[33];
      TestArrayAccessors(arr.get(), i,                  // Array and index.
                         FXSYS_itoa(vals[i], buf, 10),  // String value.
                         nullptr,                       // Const string value.
                         vals[i],                       // Integer value.
                         vals[i],                       // Float value.
                         nullptr,                       // Array value.
                         nullptr,                       // Dictionary value.
                         nullptr);                      // Stream value.
    }
  }
  {
    // Float array.
    float vals[] = {0.0f,    0,     10,    10.0f,   0.0345f,
                    897.34f, -2.5f, -1.0f, -345.0f, -0.0f};
    const char* expected_str[] = {"0",      "0",    "10", "10",   "0.0345",
                                  "897.34", "-2.5", "-1", "-345", "0"};
    ScopedArray arr(new CPDF_Array);
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      arr->InsertAt(i, new CPDF_Number(vals[i]));
    }
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      TestArrayAccessors(arr.get(), i,     // Array and index.
                         expected_str[i],  // String value.
                         nullptr,          // Const string value.
                         vals[i],          // Integer value.
                         vals[i],          // Float value.
                         nullptr,          // Array value.
                         nullptr,          // Dictionary value.
                         nullptr);         // Stream value.
    }
  }
  {
    // String and name array
    const char* const vals[] = {"this", "adsde$%^", "\r\t",           "\"012",
                                ".",    "EYREW",    "It is a joke :)"};
    ScopedArray string_array(new CPDF_Array);
    ScopedArray name_array(new CPDF_Array);
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      string_array->InsertAt(i, new CPDF_String(vals[i], false));
      name_array->InsertAt(i, new CPDF_Name(vals[i]));
    }
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      TestArrayAccessors(string_array.get(), i,  // Array and index.
                         vals[i],                // String value.
                         vals[i],                // Const string value.
                         0,                      // Integer value.
                         0,                      // Float value.
                         nullptr,                // Array value.
                         nullptr,                // Dictionary value.
                         nullptr);               // Stream value.
      TestArrayAccessors(name_array.get(), i,    // Array and index.
                         vals[i],                // String value.
                         vals[i],                // Const string value.
                         0,                      // Integer value.
                         0,                      // Float value.
                         nullptr,                // Array value.
                         nullptr,                // Dictionary value.
                         nullptr);               // Stream value.
    }
  }
  {
    // Null element array.
    ScopedArray arr(new CPDF_Array);
    for (size_t i = 0; i < 3; ++i)
      arr->InsertAt(i, new CPDF_Null);
    for (size_t i = 0; i < 3; ++i) {
      TestArrayAccessors(arr.get(), i,  // Array and index.
                         "",            // String value.
                         nullptr,       // Const string value.
                         0,             // Integer value.
                         0,             // Float value.
                         nullptr,       // Array value.
                         nullptr,       // Dictionary value.
                         nullptr);      // Stream value.
    }
  }
  {
    // Array of array.
    CPDF_Array* vals[3];
    ScopedArray arr(new CPDF_Array);
    for (size_t i = 0; i < 3; ++i) {
      vals[i] = new CPDF_Array;
      for (size_t j = 0; j < 3; ++j) {
        int value = j + 100;
        vals[i]->InsertAt(i, new CPDF_Number(value));
      }
      arr->InsertAt(i, vals[i]);
    }
    for (size_t i = 0; i < 3; ++i) {
      TestArrayAccessors(arr.get(), i,  // Array and index.
                         "",            // String value.
                         nullptr,       // Const string value.
                         0,             // Integer value.
                         0,             // Float value.
                         vals[i],       // Array value.
                         nullptr,       // Dictionary value.
                         nullptr);      // Stream value.
    }
  }
  {
    // Dictionary array.
    CPDF_Dictionary* vals[3];
    ScopedArray arr(new CPDF_Array);
    for (size_t i = 0; i < 3; ++i) {
      vals[i] = new CPDF_Dictionary;
      for (size_t j = 0; j < 3; ++j) {
        std::string key("key");
        char buf[33];
        key.append(FXSYS_itoa(j, buf, 10));
        int value = j + 200;
        vals[i]->SetAt(CFX_ByteStringC(key.c_str()), new CPDF_Number(value));
      }
      arr->InsertAt(i, vals[i]);
    }
    for (size_t i = 0; i < 3; ++i) {
      TestArrayAccessors(arr.get(), i,  // Array and index.
                         "",            // String value.
                         nullptr,       // Const string value.
                         0,             // Integer value.
                         0,             // Float value.
                         nullptr,       // Array value.
                         vals[i],       // Dictionary value.
                         nullptr);      // Stream value.
    }
  }
  {
    // Stream array.
    CPDF_Dictionary* vals[3];
    CPDF_Stream* stream_vals[3];
    ScopedArray arr(new CPDF_Array);
    for (size_t i = 0; i < 3; ++i) {
      vals[i] = new CPDF_Dictionary;
      for (size_t j = 0; j < 3; ++j) {
        std::string key("key");
        char buf[33];
        key.append(FXSYS_itoa(j, buf, 10));
        int value = j + 200;
        vals[i]->SetAt(CFX_ByteStringC(key.c_str()), new CPDF_Number(value));
      }
      uint8_t content[] = "content: this is a stream";
      size_t data_size = FX_ArraySize(content);
      uint8_t* data = reinterpret_cast<uint8_t*>(malloc(data_size));
      memcpy(data, content, data_size);
      stream_vals[i] = new CPDF_Stream(data, data_size, vals[i]);
      arr->InsertAt(i, stream_vals[i]);
    }
    for (size_t i = 0; i < 3; ++i) {
      TestArrayAccessors(arr.get(), i,     // Array and index.
                         "",               // String value.
                         nullptr,          // Const string value.
                         0,                // Integer value.
                         0,                // Float value.
                         nullptr,          // Array value.
                         vals[i],          // Dictionary value.
                         stream_vals[i]);  // Stream value.
    }
  }
  {
    // Mixed array.
    ScopedArray arr(new CPDF_Array);
    // Array arr will take ownership of all the objects inserted.
    arr->InsertAt(0, new CPDF_Boolean(true));
    arr->InsertAt(1, new CPDF_Boolean(false));
    arr->InsertAt(2, new CPDF_Number(0));
    arr->InsertAt(3, new CPDF_Number(-1234));
    arr->InsertAt(4, new CPDF_Number(2345.0f));
    arr->InsertAt(5, new CPDF_Number(0.05f));
    arr->InsertAt(6, new CPDF_String("", false));
    arr->InsertAt(7, new CPDF_String("It is a test!", false));
    arr->InsertAt(8, new CPDF_Name("NAME"));
    arr->InsertAt(9, new CPDF_Name("test"));
    arr->InsertAt(10, new CPDF_Null());
    CPDF_Array* arr_val = new CPDF_Array;
    arr_val->AddNumber(1);
    arr_val->AddNumber(2);
    arr->InsertAt(11, arr_val);
    CPDF_Dictionary* dict_val = new CPDF_Dictionary;
    dict_val->SetAt("key1", new CPDF_String("Linda", false));
    dict_val->SetAt("key2", new CPDF_String("Zoe", false));
    arr->InsertAt(12, dict_val);
    CPDF_Dictionary* stream_dict = new CPDF_Dictionary;
    stream_dict->SetAt("key1", new CPDF_String("John", false));
    stream_dict->SetAt("key2", new CPDF_String("King", false));
    uint8_t data[] = "A stream for test";
    // The data buffer will be owned by stream object, so it needs to be
    // dynamically allocated.
    size_t buf_size = sizeof(data);
    uint8_t* buf = reinterpret_cast<uint8_t*>(malloc(buf_size));
    memcpy(buf, data, buf_size);
    CPDF_Stream* stream_val = new CPDF_Stream(buf, buf_size, stream_dict);
    arr->InsertAt(13, stream_val);
    const char* const expected_str[] = {
        "true",          "false", "0",    "-1234", "2345", "0.05", "",
        "It is a test!", "NAME",  "test", "",      "",     "",     ""};
    const char* const expected_cstr[] = {
        nullptr,         nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        "It is a test!", "NAME",  "test",  nullptr, nullptr, nullptr, nullptr};
    const int expected_int[] = {1, 0, 0, -1234, 2345, 0, 0,
                                0, 0, 0, 0,     0,    0, 0};
    const float expected_float[] = {0, 0, 0, -1234, 2345, 0.05f, 0,
                                    0, 0, 0, 0,     0,    0,     0};
    for (size_t i = 0; i < arr->GetCount(); ++i) {
      EXPECT_STREQ(expected_str[i], arr->GetStringAt(i).c_str());
      EXPECT_STREQ(expected_cstr[i], arr->GetConstStringAt(i).c_str());
      EXPECT_EQ(expected_int[i], arr->GetIntegerAt(i));
      EXPECT_EQ(expected_float[i], arr->GetNumberAt(i));
      EXPECT_EQ(expected_float[i], arr->GetFloatAt(i));
      if (i == 11)
        EXPECT_EQ(arr_val, arr->GetArrayAt(i));
      else
        EXPECT_EQ(nullptr, arr->GetArrayAt(i));
      if (i == 13) {
        EXPECT_EQ(stream_dict, arr->GetDictAt(i));
        EXPECT_EQ(stream_val, arr->GetStreamAt(i));
      } else {
        EXPECT_EQ(nullptr, arr->GetStreamAt(i));
        if (i == 12)
          EXPECT_EQ(dict_val, arr->GetDictAt(i));
        else
          EXPECT_EQ(nullptr, arr->GetDictAt(i));
      }
    }
  }
}

TEST(PDFArrayTest, AddNumber) {
  float vals[] = {1.0f,         -1.0f, 0,    0.456734f,
                  12345.54321f, 0.5f,  1000, 0.000045f};
  ScopedArray arr(new CPDF_Array);
  for (size_t i = 0; i < FX_ArraySize(vals); ++i)
    arr->AddNumber(vals[i]);
  for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
    EXPECT_EQ(CPDF_Object::NUMBER, arr->GetObjectAt(i)->GetType());
    EXPECT_EQ(vals[i], arr->GetObjectAt(i)->GetNumber());
  }
}

TEST(PDFArrayTest, AddInteger) {
  int vals[] = {0, 1, 934435456, 876, 10000, -1, -24354656, -100};
  ScopedArray arr(new CPDF_Array);
  for (size_t i = 0; i < FX_ArraySize(vals); ++i)
    arr->AddInteger(vals[i]);
  for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
    EXPECT_EQ(CPDF_Object::NUMBER, arr->GetObjectAt(i)->GetType());
    EXPECT_EQ(vals[i], arr->GetObjectAt(i)->GetNumber());
  }
}

TEST(PDFArrayTest, AddStringAndName) {
  const char* vals[] = {"",        "a", "ehjhRIOYTTFdfcdnv",  "122323",
                        "$#%^&**", " ", "This is a test.\r\n"};
  ScopedArray string_array(new CPDF_Array);
  ScopedArray name_array(new CPDF_Array);
  for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
    string_array->AddString(vals[i]);
    name_array->AddName(vals[i]);
  }
  for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
    EXPECT_EQ(CPDF_Object::STRING, string_array->GetObjectAt(i)->GetType());
    EXPECT_STREQ(vals[i], string_array->GetObjectAt(i)->GetString().c_str());
    EXPECT_EQ(CPDF_Object::NAME, name_array->GetObjectAt(i)->GetType());
    EXPECT_STREQ(vals[i], name_array->GetObjectAt(i)->GetString().c_str());
  }
}

TEST(PDFArrayTest, AddReferenceAndGetObjectAt) {
  std::unique_ptr<CPDF_IndirectObjectHolder> holder(
      new CPDF_IndirectObjectHolder(nullptr));
  CPDF_Boolean* boolean_obj = new CPDF_Boolean(true);
  CPDF_Number* int_obj = new CPDF_Number(-1234);
  CPDF_Number* float_obj = new CPDF_Number(2345.089f);
  CPDF_String* str_obj = new CPDF_String("Adsfdsf 343434 %&&*\n", false);
  CPDF_Name* name_obj = new CPDF_Name("Title:");
  CPDF_Null* null_obj = new CPDF_Null();
  CPDF_Object* indirect_objs[] = {boolean_obj, int_obj,  float_obj,
                                  str_obj,     name_obj, null_obj};
  unsigned int obj_nums[] = {2, 4, 7, 2345, 799887, 1};
  ScopedArray arr(new CPDF_Array);
  ScopedArray arr1(new CPDF_Array);
  // Create two arrays of references by different AddReference() APIs.
  for (size_t i = 0; i < FX_ArraySize(indirect_objs); ++i) {
    // All the indirect objects inserted will be owned by holder.
    holder->InsertIndirectObject(obj_nums[i], indirect_objs[i]);
    arr->AddReference(holder.get(), obj_nums[i]);
    arr1->AddReference(holder.get(), indirect_objs[i]);
  }
  // Check indirect objects.
  for (size_t i = 0; i < FX_ArraySize(obj_nums); ++i)
    EXPECT_EQ(indirect_objs[i], holder->GetIndirectObject(obj_nums[i]));
  // Check arrays.
  EXPECT_EQ(arr->GetCount(), arr1->GetCount());
  for (size_t i = 0; i < arr->GetCount(); ++i) {
    EXPECT_EQ(CPDF_Object::REFERENCE, arr->GetObjectAt(i)->GetType());
    EXPECT_EQ(indirect_objs[i], arr->GetObjectAt(i)->GetDirect());
    EXPECT_EQ(indirect_objs[i], arr->GetDirectObjectAt(i));
    EXPECT_EQ(CPDF_Object::REFERENCE, arr1->GetObjectAt(i)->GetType());
    EXPECT_EQ(indirect_objs[i], arr1->GetObjectAt(i)->GetDirect());
    EXPECT_EQ(indirect_objs[i], arr1->GetDirectObjectAt(i));
  }
}

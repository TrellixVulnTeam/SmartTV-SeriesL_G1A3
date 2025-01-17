// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2008 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "xfa/fxbarcode/common/BC_CommonByteMatrix.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoder.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderBitVector.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderMaskUtil.h"
#include "xfa/fxbarcode/qrcode/BC_QRCoderMatrixUtil.h"
#include "xfa/fxbarcode/utils.h"

const int32_t CBC_QRCoderMatrixUtil::POSITION_DETECTION_PATTERN[7][7] = {
    {1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1}, {1, 0, 1, 1, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1}};
const int32_t CBC_QRCoderMatrixUtil::HORIZONTAL_SEPARATION_PATTERN[1][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0}};
const int32_t CBC_QRCoderMatrixUtil::VERTICAL_SEPARATION_PATTERN[7][1] = {
    {0}, {0}, {0}, {0}, {0}, {0}, {0}};
const int32_t CBC_QRCoderMatrixUtil::POSITION_ADJUSTMENT_PATTERN[5][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1}};
const int32_t
    CBC_QRCoderMatrixUtil::POSITION_ADJUSTMENT_PATTERN_COORDINATE_TABLE[40][7] =
        // NOLINTNEXTLINE
    {
        {-1, -1, -1, -1, -1, -1, -1},   {6, 18, -1, -1, -1, -1, -1},
        {6, 22, -1, -1, -1, -1, -1},    {6, 26, -1, -1, -1, -1, -1},
        {6, 30, -1, -1, -1, -1, -1},    {6, 34, -1, -1, -1, -1, -1},
        {6, 22, 38, -1, -1, -1, -1},    {6, 24, 42, -1, -1, -1, -1},
        {6, 26, 46, -1, -1, -1, -1},    {6, 28, 50, -1, -1, -1, -1},
        {6, 30, 54, -1, -1, -1, -1},    {6, 32, 58, -1, -1, -1, -1},
        {6, 34, 62, -1, -1, -1, -1},    {6, 26, 46, 66, -1, -1, -1},
        {6, 26, 48, 70, -1, -1, -1},    {6, 26, 50, 74, -1, -1, -1},
        {6, 30, 54, 78, -1, -1, -1},    {6, 30, 56, 82, -1, -1, -1},
        {6, 30, 58, 86, -1, -1, -1},    {6, 34, 62, 90, -1, -1, -1},
        {6, 28, 50, 72, 94, -1, -1},    {6, 26, 50, 74, 98, -1, -1},
        {6, 30, 54, 78, 102, -1, -1},   {6, 28, 54, 80, 106, -1, -1},
        {6, 32, 58, 84, 110, -1, -1},   {6, 30, 58, 86, 114, -1, -1},
        {6, 34, 62, 90, 118, -1, -1},   {6, 26, 50, 74, 98, 122, -1},
        {6, 30, 54, 78, 102, 126, -1},  {6, 26, 52, 78, 104, 130, -1},
        {6, 30, 56, 82, 108, 134, -1},  {6, 34, 60, 86, 112, 138, -1},
        {6, 30, 58, 86, 114, 142, -1},  {6, 34, 62, 90, 118, 146, -1},
        {6, 30, 54, 78, 102, 126, 150}, {6, 24, 50, 76, 102, 128, 154},
        {6, 28, 54, 80, 106, 132, 158}, {6, 32, 58, 84, 110, 136, 162},
        {6, 26, 54, 82, 110, 138, 166}, {6, 30, 58, 86, 114, 142, 170},
};
const int32_t CBC_QRCoderMatrixUtil::TYPE_INFO_COORDINATES[15][2] = {
    {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5}, {8, 7}, {8, 8},
    {7, 8}, {5, 8}, {4, 8}, {3, 8}, {2, 8}, {1, 8}, {0, 8},
};
const int32_t CBC_QRCoderMatrixUtil::VERSION_INFO_POLY = 0x1f25;
const int32_t CBC_QRCoderMatrixUtil::TYPE_INFO_POLY = 0x0537;
const int32_t CBC_QRCoderMatrixUtil::TYPE_INFO_MASK_PATTERN = 0x5412;

void CBC_QRCoderMatrixUtil::ClearMatrix(CBC_CommonByteMatrix* matrix,
                                        int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  matrix->clear((uint8_t)-1);
}
void CBC_QRCoderMatrixUtil::BuildMatrix(
    CBC_QRCoderBitVector* dataBits,
    CBC_QRCoderErrorCorrectionLevel* ecLevel,
    int32_t version,
    int32_t maskPattern,
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  ClearMatrix(matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedBasicPatterns(version, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedTypeInfo(ecLevel, maskPattern, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  MaybeEmbedVersionInfo(version, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedDataBits(dataBits, maskPattern, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
}
void CBC_QRCoderMatrixUtil::EmbedBasicPatterns(int32_t version,
                                               CBC_CommonByteMatrix* matrix,
                                               int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  EmbedPositionDetectionPatternsAndSeparators(matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedDarkDotAtLeftBottomCorner(matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  MaybeEmbedPositionAdjustmentPatterns(version, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedTimingPatterns(matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
}
void CBC_QRCoderMatrixUtil::EmbedTypeInfo(
    CBC_QRCoderErrorCorrectionLevel* ecLevel,
    int32_t maskPattern,
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  CBC_QRCoderBitVector typeInfoBits;
  typeInfoBits.Init();
  MakeTypeInfoBits(ecLevel, maskPattern, &typeInfoBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  for (int32_t i = 0; i < typeInfoBits.Size(); i++) {
    int32_t bit = typeInfoBits.At(typeInfoBits.Size() - 1 - i, e);
    BC_EXCEPTION_CHECK_ReturnVoid(e);
    int32_t x1 = TYPE_INFO_COORDINATES[i][0];
    int32_t y1 = TYPE_INFO_COORDINATES[i][1];
    matrix->Set(x1, y1, bit);
    if (i < 8) {
      int32_t x2 = matrix->GetWidth() - i - 1;
      int32_t y2 = 8;
      matrix->Set(x2, y2, bit);
    } else {
      int32_t x2 = 8;
      int32_t y2 = matrix->GetHeight() - 7 + (i - 8);
      matrix->Set(x2, y2, bit);
    }
  }
}
void CBC_QRCoderMatrixUtil::MaybeEmbedVersionInfo(int32_t version,
                                                  CBC_CommonByteMatrix* matrix,
                                                  int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  if (version < 7) {
    return;
  }
  CBC_QRCoderBitVector versionInfoBits;
  versionInfoBits.Init();
  MakeVersionInfoBits(version, &versionInfoBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  int32_t bitIndex = 6 * 3 - 1;
  for (int32_t i = 0; i < 6; i++) {
    for (int32_t j = 0; j < 3; j++) {
      int32_t bit = versionInfoBits.At(bitIndex, e);
      BC_EXCEPTION_CHECK_ReturnVoid(e);
      bitIndex--;
      matrix->Set(i, matrix->GetHeight() - 11 + j, bit);
      matrix->Set(matrix->GetHeight() - 11 + j, i, bit);
    }
  }
}
void CBC_QRCoderMatrixUtil::EmbedDataBits(CBC_QRCoderBitVector* dataBits,
                                          int32_t maskPattern,
                                          CBC_CommonByteMatrix* matrix,
                                          int32_t& e) {
  if (matrix == NULL || dataBits == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  int32_t bitIndex = 0;
  int32_t direction = -1;
  int32_t x = matrix->GetWidth() - 1;
  int32_t y = matrix->GetHeight() - 1;
  while (x > 0) {
    if (x == 6) {
      x -= 1;
    }
    while (y >= 0 && y < matrix->GetHeight()) {
      if (y == 6) {
        y += direction;
        continue;
      }
      for (int32_t i = 0; i < 2; i++) {
        int32_t xx = x - i;
        if (!IsEmpty(matrix->Get(xx, y))) {
          continue;
        }
        int32_t bit;
        if (bitIndex < dataBits->Size()) {
          bit = dataBits->At(bitIndex, e);
          BC_EXCEPTION_CHECK_ReturnVoid(e);
          bitIndex++;
        } else {
          bit = 0;
        }
        if (maskPattern != -1) {
          FX_BOOL bol =
              CBC_QRCoderMaskUtil::GetDataMaskBit(maskPattern, xx, y, e);
          BC_EXCEPTION_CHECK_ReturnVoid(e);
          if (bol) {
            bit ^= 0x01;
          }
        }
        matrix->Set(xx, y, bit);
      }
      y += direction;
    }
    direction = -direction;
    y += direction;
    x -= 2;
  }
  if (bitIndex != dataBits->Size()) {
    return;
  }
}
int32_t CBC_QRCoderMatrixUtil::CalculateBCHCode(int32_t value, int32_t poly) {
  int32_t msbSetInPoly = FindMSBSet(poly);
  value <<= msbSetInPoly - 1;
  while (FindMSBSet(value) >= msbSetInPoly) {
    value ^= poly << (FindMSBSet(value) - msbSetInPoly);
  }
  return value;
}
void CBC_QRCoderMatrixUtil::MakeTypeInfoBits(
    CBC_QRCoderErrorCorrectionLevel* ecLevel,
    int32_t maskPattern,
    CBC_QRCoderBitVector* bits,
    int32_t& e) {
  if (bits == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  if (!CBC_QRCoder::IsValidMaskPattern(maskPattern)) {
    e = BCExceptionBadMask;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  int32_t typeInfo = (ecLevel->GetBits() << 3) | maskPattern;
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  bits->AppendBits(typeInfo, 5, e);
  int32_t bchCode = CalculateBCHCode(typeInfo, TYPE_INFO_POLY);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  bits->AppendBits(bchCode, 10, e);
  CBC_QRCoderBitVector maskBits;
  maskBits.Init();
  maskBits.AppendBits(TYPE_INFO_MASK_PATTERN, 15, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  bits->XOR(&maskBits, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  if (bits->Size() != 15) {
    e = BCExceptionBitSizeNot15;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
void CBC_QRCoderMatrixUtil::MakeVersionInfoBits(int32_t version,
                                                CBC_QRCoderBitVector* bits,
                                                int32_t& e) {
  if (bits == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  bits->AppendBits(version, 6, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  int32_t bchCode = CalculateBCHCode(version, VERSION_INFO_POLY);
  bits->AppendBits(bchCode, 12, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  if (bits->Size() != 18) {
    e = BCExceptionBitSizeNot18;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
}
FX_BOOL CBC_QRCoderMatrixUtil::IsEmpty(int32_t value) {
  return (uint8_t)value == 0xff;
}
FX_BOOL CBC_QRCoderMatrixUtil::IsValidValue(int32_t value) {
  return ((uint8_t)value == 0xff || (uint8_t)value == 0x00 ||
          (uint8_t)value == 0x01);
}
void CBC_QRCoderMatrixUtil::EmbedTimingPatterns(CBC_CommonByteMatrix* matrix,
                                                int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  for (int32_t i = 8; i < matrix->GetWidth() - 8; i++) {
    int32_t bit = (i + 1) % 2;
    if (!IsValidValue(matrix->Get(i, 6))) {
      e = BCExceptionInvalidateImageData;
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    }
    if (IsEmpty(matrix->Get(i, 6))) {
      matrix->Set(i, 6, bit);
    }
    if (!IsValidValue(matrix->Get(6, i))) {
      e = BCExceptionInvalidateImageData;
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    }
    if (IsEmpty(matrix->Get(6, i))) {
      matrix->Set(6, i, bit);
    }
  }
}
void CBC_QRCoderMatrixUtil::EmbedDarkDotAtLeftBottomCorner(
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  if (matrix->Get(8, matrix->GetHeight() - 8) == 0) {
    e = BCExceptionHeight_8BeZero;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  matrix->Set(8, matrix->GetHeight() - 8, 1);
}
void CBC_QRCoderMatrixUtil::EmbedHorizontalSeparationPattern(
    int32_t xStart,
    int32_t yStart,
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  for (int32_t x = 0; x < 8; x++) {
    if (!IsEmpty(matrix->Get(xStart + x, yStart))) {
      e = BCExceptionInvalidateData;
      BC_EXCEPTION_CHECK_ReturnVoid(e)
    }
    matrix->Set(xStart + x, yStart, HORIZONTAL_SEPARATION_PATTERN[0][x]);
  }
}
void CBC_QRCoderMatrixUtil::EmbedVerticalSeparationPattern(
    int32_t xStart,
    int32_t yStart,
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  for (int32_t y = 0; y < 7; y++) {
    if (!IsEmpty(matrix->Get(xStart, yStart + y))) {
      e = BCExceptionInvalidateData;
      BC_EXCEPTION_CHECK_ReturnVoid(e);
    }
    matrix->Set(xStart, yStart + y, VERTICAL_SEPARATION_PATTERN[y][0]);
  }
}
void CBC_QRCoderMatrixUtil::EmbedPositionAdjustmentPattern(
    int32_t xStart,
    int32_t yStart,
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  for (int32_t y = 0; y < 5; y++) {
    for (int32_t x = 0; x < 5; x++) {
      if (!IsEmpty(matrix->Get(xStart + x, y + yStart))) {
        e = BCExceptionInvalidateData;
        BC_EXCEPTION_CHECK_ReturnVoid(e);
      }
      matrix->Set(xStart + x, yStart + y, POSITION_ADJUSTMENT_PATTERN[y][x]);
    }
  }
}
void CBC_QRCoderMatrixUtil::EmbedPositionDetectionPattern(
    int32_t xStart,
    int32_t yStart,
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  for (int32_t y = 0; y < 7; y++) {
    for (int32_t x = 0; x < 7; x++) {
      if (!IsEmpty(matrix->Get(xStart + x, yStart + y))) {
        e = BCExceptionInvalidateData;
        BC_EXCEPTION_CHECK_ReturnVoid(e);
      }
      matrix->Set(xStart + x, yStart + y, POSITION_DETECTION_PATTERN[y][x]);
    }
  }
}
void CBC_QRCoderMatrixUtil::EmbedPositionDetectionPatternsAndSeparators(
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  int32_t pdpWidth = 7;
  EmbedPositionDetectionPattern(0, 0, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedPositionDetectionPattern(matrix->GetWidth() - pdpWidth, 0, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedPositionDetectionPattern(0, matrix->GetWidth() - pdpWidth, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  int32_t hspWidth = 8;
  EmbedHorizontalSeparationPattern(0, hspWidth - 1, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedHorizontalSeparationPattern(matrix->GetWidth() - hspWidth, hspWidth - 1,
                                   matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedHorizontalSeparationPattern(0, matrix->GetWidth() - hspWidth, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  int32_t vspSize = 7;
  EmbedVerticalSeparationPattern(vspSize, 0, matrix, e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedVerticalSeparationPattern(matrix->GetHeight() - vspSize - 1, 0, matrix,
                                 e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
  EmbedVerticalSeparationPattern(vspSize, matrix->GetHeight() - vspSize, matrix,
                                 e);
  BC_EXCEPTION_CHECK_ReturnVoid(e);
}
void CBC_QRCoderMatrixUtil::MaybeEmbedPositionAdjustmentPatterns(
    int32_t version,
    CBC_CommonByteMatrix* matrix,
    int32_t& e) {
  if (matrix == NULL) {
    e = BCExceptionNullPointer;
    BC_EXCEPTION_CHECK_ReturnVoid(e);
  }
  if (version < 2) {
    return;
  }
  int32_t index = version - 1;
  int32_t const* coordinates =
      &(POSITION_ADJUSTMENT_PATTERN_COORDINATE_TABLE[index][0]);
  int32_t numCoordinate = 7;
  for (int32_t i = 0; i < numCoordinate; i++) {
    for (int32_t j = 0; j < numCoordinate; j++) {
      int32_t y = coordinates[i];
      int32_t x = coordinates[j];
      if (x == -1 || y == -1) {
        continue;
      }
      if (IsEmpty(matrix->Get(x, y))) {
        EmbedPositionAdjustmentPattern(x - 2, y - 2, matrix, e);
        BC_EXCEPTION_CHECK_ReturnVoid(e);
      }
    }
  }
}
int32_t CBC_QRCoderMatrixUtil::FindMSBSet(int32_t value) {
  int32_t numDigits = 0;
  while (value != 0) {
    value >>= 1;
    ++numDigits;
  }
  return numDigits;
}
CBC_QRCoderMatrixUtil::CBC_QRCoderMatrixUtil() {}
CBC_QRCoderMatrixUtil::~CBC_QRCoderMatrixUtil() {}

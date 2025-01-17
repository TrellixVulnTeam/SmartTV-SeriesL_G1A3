// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/fx_arabic.h"
#include "core/fxcrt/include/fx_ucd.h"

namespace {

const FX_ARBFORMTABLE g_FX_ArabicFormTables[] = {
    {0xFE81, 0xFE82, 0xFE81, 0xFE82}, {0xFE83, 0xFE84, 0xFE83, 0xFE84},
    {0xFE85, 0xFE86, 0xFE85, 0xFE86}, {0xFE87, 0xFE88, 0xFE87, 0xFE88},
    {0xFE89, 0xFE8A, 0xFE8B, 0xFE8C}, {0xFE8D, 0xFE8E, 0xFE8D, 0xFE8E},
    {0xFE8F, 0xFE90, 0xFE91, 0xFE92}, {0xFE93, 0xFE94, 0xFE93, 0xFE94},
    {0xFE95, 0xFE96, 0xFE97, 0xFE98}, {0xFE99, 0xFE9A, 0xFE9B, 0xFE9C},
    {0xFE9D, 0xFE9E, 0xFE9F, 0xFEA0}, {0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4},
    {0xFEA5, 0xFEA6, 0xFEA7, 0xFEA8}, {0xFEA9, 0xFEAA, 0xFEA9, 0xFEAA},
    {0xFEAB, 0xFEAC, 0xFEAB, 0xFEAC}, {0xFEAD, 0xFEAE, 0xFEAD, 0xFEAE},
    {0xFEAF, 0xFEB0, 0xFEAF, 0xFEB0}, {0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4},
    {0xFEB5, 0xFEB6, 0xFEB7, 0xFEB8}, {0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC},
    {0xFEBD, 0xFEBE, 0xFEBF, 0xFEC0}, {0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4},
    {0xFEC5, 0xFEC6, 0xFEC7, 0xFEC8}, {0xFEC9, 0xFECA, 0xFECB, 0xFECC},
    {0xFECD, 0xFECE, 0xFECF, 0xFED0}, {0x063B, 0x063B, 0x063B, 0x063B},
    {0x063C, 0x063C, 0x063C, 0x063C}, {0x063D, 0x063D, 0x063D, 0x063D},
    {0x063E, 0x063E, 0x063E, 0x063E}, {0x063F, 0x063F, 0x063F, 0x063F},
    {0x0640, 0x0640, 0x0640, 0x0640}, {0xFED1, 0xFED2, 0xFED3, 0xFED4},
    {0xFED5, 0xFED6, 0xFED7, 0xFED8}, {0xFED9, 0xFEDA, 0xFEDB, 0xFEDC},
    {0xFEDD, 0xFEDE, 0xFEDF, 0xFEE0}, {0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4},
    {0xFEE5, 0xFEE6, 0xFEE7, 0xFEE8}, {0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC},
    {0xFEED, 0xFEEE, 0xFEED, 0xFEEE}, {0xFEEF, 0xFEF0, 0xFBFE, 0xFBFF},
    {0xFEF1, 0xFEF2, 0xFEF3, 0xFEF4}, {0x064B, 0x064B, 0x064B, 0x064B},
    {0x064C, 0x064C, 0x064C, 0x064C}, {0x064D, 0x064D, 0x064D, 0x064D},
    {0x064E, 0x064E, 0x064E, 0x064E}, {0x064F, 0x064F, 0x064F, 0x064F},
    {0x0650, 0x0650, 0x0650, 0x0650}, {0x0651, 0x0651, 0x0651, 0x0651},
    {0x0652, 0x0652, 0x0652, 0x0652}, {0x0653, 0x0653, 0x0653, 0x0653},
    {0x0654, 0x0654, 0x0654, 0x0654}, {0x0655, 0x0655, 0x0655, 0x0655},
    {0x0656, 0x0656, 0x0656, 0x0656}, {0x0657, 0x0657, 0x0657, 0x0657},
    {0x0658, 0x0658, 0x0658, 0x0658}, {0x0659, 0x0659, 0x0659, 0x0659},
    {0x065A, 0x065A, 0x065A, 0x065A}, {0x065B, 0x065B, 0x065B, 0x065B},
    {0x065C, 0x065C, 0x065C, 0x065C}, {0x065D, 0x065D, 0x065D, 0x065D},
    {0x065E, 0x065E, 0x065E, 0x065E}, {0x065F, 0x065F, 0x065F, 0x065F},
    {0x0660, 0x0660, 0x0660, 0x0660}, {0x0661, 0x0661, 0x0661, 0x0661},
    {0x0662, 0x0662, 0x0662, 0x0662}, {0x0663, 0x0663, 0x0663, 0x0663},
    {0x0664, 0x0664, 0x0664, 0x0664}, {0x0665, 0x0665, 0x0665, 0x0665},
    {0x0666, 0x0666, 0x0666, 0x0666}, {0x0667, 0x0667, 0x0667, 0x0667},
    {0x0668, 0x0668, 0x0668, 0x0668}, {0x0669, 0x0669, 0x0669, 0x0669},
    {0x066A, 0x066A, 0x066A, 0x066A}, {0x066B, 0x066B, 0x066B, 0x066B},
    {0x066C, 0x066C, 0x066C, 0x066C}, {0x066D, 0x066D, 0x066D, 0x066D},
    {0x066E, 0x066E, 0x066E, 0x066E}, {0x066F, 0x066F, 0x066F, 0x066F},
    {0x0670, 0x0670, 0x0670, 0x0670}, {0xFB50, 0xFB51, 0xFB50, 0xFB51},
    {0x0672, 0x0672, 0x0672, 0x0672}, {0x0673, 0x0673, 0x0673, 0x0673},
    {0x0674, 0x0674, 0x0674, 0x0674}, {0x0675, 0x0675, 0x0675, 0x0675},
    {0x0676, 0x0676, 0x0676, 0x0676}, {0x0677, 0x0677, 0x0677, 0x0677},
    {0x0678, 0x0678, 0x0678, 0x0678}, {0xFB66, 0xFB67, 0xFB68, 0xFB69},
    {0xFB5E, 0xFB5F, 0xFB60, 0xFB61}, {0xFB52, 0xFB53, 0xFB54, 0xFB55},
    {0x067C, 0x067C, 0x067C, 0x067C}, {0x067D, 0x067D, 0x067D, 0x067D},
    {0xFB56, 0xFB57, 0xFB58, 0xFB59}, {0xFB62, 0xFB63, 0xFB64, 0xFB65},
    {0xFB5A, 0xFB5B, 0xFB5C, 0xFB5D}, {0x0681, 0x0681, 0x0681, 0x0681},
    {0x0682, 0x0682, 0x0682, 0x0682}, {0xFB76, 0xFB77, 0xFB78, 0xFB79},
    {0xFB72, 0xFB73, 0xFB74, 0xFB75}, {0x0685, 0x0685, 0x0685, 0x0685},
    {0xFB7A, 0xFB7B, 0xFB7C, 0xFB7D}, {0xFB7E, 0xFB7F, 0xFB80, 0xFB81},
    {0xFB88, 0xFB89, 0xFB88, 0xFB89}, {0x0689, 0x0689, 0x0689, 0x0689},
    {0x068A, 0x068A, 0x068A, 0x068A}, {0x068B, 0x068B, 0x068B, 0x068B},
    {0xFB84, 0xFB85, 0xFB84, 0xFB85}, {0xFB82, 0xFB83, 0xFB82, 0xFB83},
    {0xFB86, 0xFB87, 0xFB86, 0xFB87}, {0x068F, 0x068F, 0x068F, 0x068F},
    {0x0690, 0x0690, 0x0690, 0x0690}, {0xFB8C, 0xFB8D, 0xFB8C, 0xFB8D},
    {0x0692, 0x0692, 0x0692, 0x0692}, {0x0693, 0x0693, 0x0693, 0x0693},
    {0x0694, 0x0694, 0x0694, 0x0694}, {0x0695, 0x0695, 0x0695, 0x0695},
    {0x0696, 0x0696, 0x0696, 0x0696}, {0x0697, 0x0697, 0x0697, 0x0697},
    {0xFB8A, 0xFB8B, 0xFB8A, 0xFB8B}, {0x0699, 0x0699, 0x0699, 0x0699},
    {0x069A, 0x069A, 0x069A, 0x069A}, {0x069B, 0x069B, 0x069B, 0x069B},
    {0x069C, 0x069C, 0x069C, 0x069C}, {0x069D, 0x069D, 0x069D, 0x069D},
    {0x069E, 0x069E, 0x069E, 0x069E}, {0x069F, 0x069F, 0x069F, 0x069F},
    {0x06A0, 0x06A0, 0x06A0, 0x06A0}, {0x06A1, 0x06A1, 0x06A1, 0x06A1},
    {0x06A2, 0x06A2, 0x06A2, 0x06A2}, {0x06A3, 0x06A3, 0x06A3, 0x06A3},
    {0xFB6A, 0xFB6B, 0xFB6C, 0xFB6D}, {0x06A5, 0x06A5, 0x06A5, 0x06A5},
    {0xFB6E, 0xFB6F, 0xFB70, 0xFB71}, {0x06A7, 0x06A7, 0x06A7, 0x06A7},
    {0x06A8, 0x06A8, 0x06A8, 0x06A8}, {0xFB8E, 0xFB8F, 0xFB90, 0xFB91},
    {0x06AA, 0x06AA, 0x06AA, 0x06AA}, {0x06AB, 0x06AB, 0x06AB, 0x06AB},
    {0x06AC, 0x06AC, 0x06AC, 0x06AC}, {0xFBD3, 0xFBD4, 0xFBD5, 0xFBD6},
    {0x06AE, 0x06AE, 0x06AE, 0x06AE}, {0xFB92, 0xFB93, 0xFB94, 0xFB95},
    {0x06B0, 0x06B0, 0x06B0, 0x06B0}, {0xFB9A, 0xFB9B, 0xFB9C, 0xFB9D},
    {0x06B2, 0x06B2, 0x06B2, 0x06B2}, {0xFB96, 0xFB97, 0xFB98, 0xFB99},
    {0x06B4, 0x06B4, 0x06B4, 0x06B4}, {0x06B5, 0x06B5, 0x06B5, 0x06B5},
    {0x06B6, 0x06B6, 0x06B6, 0x06B6}, {0x06B7, 0x06B7, 0x06B7, 0x06B7},
    {0x06B8, 0x06B8, 0x06B8, 0x06B8}, {0x06B9, 0x06B9, 0x06B9, 0x06B9},
    {0xFB9E, 0xFB9F, 0xFBE8, 0xFBE9}, {0xFBA0, 0xFBA1, 0xFBA2, 0xFBA3},
    {0x06BC, 0x06BC, 0x06BC, 0x06BC}, {0x06BD, 0x06BD, 0x06BD, 0x06BD},
    {0xFBAA, 0xFBAB, 0xFBAC, 0xFBAD}, {0x06BF, 0x06BF, 0x06BF, 0x06BF},
    {0xFBA4, 0xFBA5, 0xFBA4, 0xFBA5}, {0xFBA6, 0xFBA7, 0xFBA8, 0xFBA9},
    {0x06C2, 0x06C2, 0x06C2, 0x06C2}, {0x06C3, 0x06C3, 0x06C3, 0x06C3},
    {0x06C4, 0x06C4, 0x06C4, 0x06C4}, {0xFBE0, 0xFBE1, 0xFBE0, 0xFBE1},
    {0xFBD9, 0xFBDA, 0xFBD9, 0xFBDA}, {0xFBD7, 0xFBD8, 0xFBD7, 0xFBD8},
    {0xFBDB, 0xFBDC, 0xFBDB, 0xFBDC}, {0xFBE2, 0xFBE3, 0xFBE2, 0xFBE3},
    {0x06CA, 0x06CA, 0x06CA, 0x06CA}, {0xFBDE, 0xFBDF, 0xFBDE, 0xFBDF},
    {0xFBFC, 0xFBFD, 0xFBFE, 0xFBFF}, {0x06CD, 0x06CD, 0x06CD, 0x06CD},
    {0x06CE, 0x06CE, 0x06CE, 0x06CE}, {0x06CF, 0x06CF, 0x06CF, 0x06CF},
    {0xFBE4, 0xFBE5, 0xFBE6, 0xFBE7}, {0x06D1, 0x06D1, 0x06D1, 0x06D1},
    {0xFBAE, 0xFBAF, 0xFBAE, 0xFBAF}, {0xFBB0, 0xFBB1, 0xFBB0, 0xFBB1},
    {0x06D4, 0x06D4, 0x06D4, 0x06D4}, {0x06D5, 0x06D5, 0x06D5, 0x06D5},
};

const FX_ARAALEF gs_FX_AlefTable[] = {
    {0x0622, 0xFEF5},
    {0x0623, 0xFEF7},
    {0x0625, 0xFEF9},
    {0x0627, 0xFEFB},
};

const FX_ARASHADDA gs_FX_ShaddaTable[] = {
    {0x064C, 0xFC5E}, {0x064D, 0xFC5F}, {0x064E, 0xFC60},
    {0x064F, 0xFC61}, {0x0650, 0xFC62},
};

}  // namespace

const FX_ARBFORMTABLE* FX_GetArabicFormTable(FX_WCHAR unicode) {
  if (unicode < 0x622 || unicode > 0x6d5) {
    return NULL;
  }
  return g_FX_ArabicFormTables + unicode - 0x622;
}
FX_WCHAR FX_GetArabicFromAlefTable(FX_WCHAR alef) {
  static const int32_t s_iAlefCount =
      sizeof(gs_FX_AlefTable) / sizeof(FX_ARAALEF);
  for (int32_t iStart = 0; iStart < s_iAlefCount; iStart++) {
    const FX_ARAALEF& v = gs_FX_AlefTable[iStart];
    if (v.wAlef == alef) {
      return v.wIsolated;
    }
  }
  return alef;
}
FX_WCHAR FX_GetArabicFromShaddaTable(FX_WCHAR shadda) {
  static const int32_t s_iShaddaCount =
      sizeof(gs_FX_ShaddaTable) / sizeof(FX_ARASHADDA);
  for (int32_t iStart = 0; iStart < s_iShaddaCount; iStart++) {
    const FX_ARASHADDA& v = gs_FX_ShaddaTable[iStart];
    if (v.wShadda == shadda) {
      return v.wIsolated;
    }
  }
  return shadda;
}

IFX_ArabicChar* IFX_ArabicChar::Create() {
  return new CFX_ArabicChar;
}
FX_BOOL CFX_ArabicChar::IsArabicChar(FX_WCHAR wch) const {
  uint32_t dwRet =
      kTextLayoutCodeProperties[(uint16_t)wch] & FX_CHARTYPEBITSMASK;
  return dwRet >= FX_CHARTYPE_ArabicAlef;
}
FX_BOOL CFX_ArabicChar::IsArabicFormChar(FX_WCHAR wch) const {
  return (kTextLayoutCodeProperties[(uint16_t)wch] & FX_CHARTYPEBITSMASK) ==
         FX_CHARTYPE_ArabicForm;
}
FX_WCHAR CFX_ArabicChar::GetFormChar(FX_WCHAR wch,
                                     FX_WCHAR prev,
                                     FX_WCHAR next) const {
  CFX_Char c(wch, kTextLayoutCodeProperties[(uint16_t)wch]);
  CFX_Char p(prev, kTextLayoutCodeProperties[(uint16_t)prev]);
  CFX_Char n(next, kTextLayoutCodeProperties[(uint16_t)next]);
  return GetFormChar(&c, &p, &n);
}
FX_WCHAR CFX_ArabicChar::GetFormChar(const CFX_Char* cur,
                                     const CFX_Char* prev,
                                     const CFX_Char* next) const {
  FX_CHARTYPE eCur;
  FX_WCHAR wCur;
  const FX_ARBFORMTABLE* ft = ParseChar(cur, wCur, eCur);
  if (eCur < FX_CHARTYPE_ArabicAlef || eCur >= FX_CHARTYPE_ArabicNormal) {
    return wCur;
  }
  FX_CHARTYPE ePrev;
  FX_WCHAR wPrev;
  ParseChar(prev, wPrev, ePrev);
  if (wPrev == 0x0644 && eCur == FX_CHARTYPE_ArabicAlef) {
    return 0xFEFF;
  }
  FX_CHARTYPE eNext;
  FX_WCHAR wNext;
  ParseChar(next, wNext, eNext);
  bool bAlef = (eNext == FX_CHARTYPE_ArabicAlef && wCur == 0x644);
  if (ePrev < FX_CHARTYPE_ArabicAlef) {
    if (bAlef) {
      return FX_GetArabicFromAlefTable(wNext);
    } else {
      return (eNext < FX_CHARTYPE_ArabicAlef) ? ft->wIsolated : ft->wInitial;
    }
  } else {
    if (bAlef) {
      wCur = FX_GetArabicFromAlefTable(wNext);
      return (ePrev != FX_CHARTYPE_ArabicDistortion) ? wCur : ++wCur;
    } else if (ePrev == FX_CHARTYPE_ArabicAlef ||
               ePrev == FX_CHARTYPE_ArabicSpecial) {
      return (eNext < FX_CHARTYPE_ArabicAlef) ? ft->wIsolated : ft->wInitial;
    } else {
      return (eNext < FX_CHARTYPE_ArabicAlef) ? ft->wFinal : ft->wMedial;
    }
  }
}
const FX_ARBFORMTABLE* CFX_ArabicChar::ParseChar(const CFX_Char* pTC,
                                                 FX_WCHAR& wChar,
                                                 FX_CHARTYPE& eType) const {
  if (pTC == NULL) {
    eType = FX_CHARTYPE_Unknown;
    wChar = 0xFEFF;
    return NULL;
  }
  eType = (FX_CHARTYPE)pTC->GetCharType();
  wChar = (FX_WCHAR)pTC->m_wCharCode;
  const FX_ARBFORMTABLE* pFT = FX_GetArabicFormTable(wChar);
  if (pFT == NULL || eType >= FX_CHARTYPE_ArabicNormal) {
    eType = FX_CHARTYPE_Unknown;
  }
  return pFT;
}
void FX_BidiReverseString(CFX_WideString& wsText,
                          int32_t iStart,
                          int32_t iCount) {
  FXSYS_assert(iStart > -1 && iStart < wsText.GetLength());
  FXSYS_assert(iCount >= 0 && iStart + iCount <= wsText.GetLength());
  FX_WCHAR wch;
  FX_WCHAR* pStart = (FX_WCHAR*)(const FX_WCHAR*)wsText;
  pStart += iStart;
  FX_WCHAR* pEnd = pStart + iCount - 1;
  while (pStart < pEnd) {
    wch = *pStart;
    *pStart++ = *pEnd;
    *pEnd-- = wch;
  }
}
void FX_BidiSetDeferredRun(CFX_Int32Array& values,
                           int32_t iStart,
                           int32_t iCount,
                           int32_t iValue) {
  FXSYS_assert(iStart > -1 && iStart <= values.GetSize());
  FXSYS_assert(iStart - iCount > -1);
  for (int32_t i = iStart - 1; i >= iStart - iCount; i--) {
    values.SetAt(i, iValue);
  }
}
const int32_t gc_FX_BidiNTypes[] = {
    FX_BIDICLASS_N,   FX_BIDICLASS_L,   FX_BIDICLASS_R,   FX_BIDICLASS_AN,
    FX_BIDICLASS_EN,  FX_BIDICLASS_AL,  FX_BIDICLASS_NSM, FX_BIDICLASS_CS,
    FX_BIDICLASS_ES,  FX_BIDICLASS_ET,  FX_BIDICLASS_BN,  FX_BIDICLASS_BN,
    FX_BIDICLASS_N,   FX_BIDICLASS_B,   FX_BIDICLASS_RLO, FX_BIDICLASS_RLE,
    FX_BIDICLASS_LRO, FX_BIDICLASS_LRE, FX_BIDICLASS_PDF, FX_BIDICLASS_ON,
};
void FX_BidiClassify(const CFX_WideString& wsText,
                     CFX_Int32Array& classes,
                     FX_BOOL bWS) {
  FXSYS_assert(wsText.GetLength() == classes.GetSize());
  int32_t iCount = wsText.GetLength();
  const FX_WCHAR* pwsStart = (const FX_WCHAR*)wsText;
  FX_WCHAR wch;
  int32_t iCls;
  if (bWS) {
    for (int32_t i = 0; i < iCount; i++) {
      wch = *pwsStart++;
      iCls =
          ((kTextLayoutCodeProperties[(uint16_t)wch] & FX_BIDICLASSBITSMASK) >>
           FX_BIDICLASSBITS);
      classes.SetAt(i, iCls);
    }
  } else {
    for (int32_t i = 0; i < iCount; i++) {
      wch = *pwsStart++;
      iCls =
          ((kTextLayoutCodeProperties[(uint16_t)wch] & FX_BIDICLASSBITSMASK) >>
           FX_BIDICLASSBITS);
      classes.SetAt(i, gc_FX_BidiNTypes[iCls]);
    }
  }
}
int32_t FX_BidiResolveExplicit(int32_t iBaseLevel,
                               int32_t iDirection,
                               CFX_Int32Array& classes,
                               CFX_Int32Array& levels,
                               int32_t iStart,
                               int32_t iCount,
                               int32_t iNest) {
  FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL && iNest >= 0);
  FXSYS_assert(classes.GetSize() == levels.GetSize());
  FXSYS_assert(iStart >= 0 && iStart < classes.GetSize());
  FXSYS_assert(iCount >= 0 && iStart + iCount <= classes.GetSize());
  if (iCount < 1) {
    return 0;
  }

  int32_t iSize = classes.GetSize();
  int32_t i = iStart, iCur, iCls;
  for (; i < iSize && iCount > 0; i++, iCount--) {
    iCur = iCls = classes.GetAt(i);

    if (iDirection != FX_BIDICLASS_N) {
      iCls = iDirection;
    }
    if (iCur != FX_BIDICLASS_BN) {
      classes.SetAt(i, iCls);
    }
    levels.SetAt(i, iBaseLevel);
  }
  return i - iStart;
}
const int32_t gc_FX_BidiWeakStates[][10] = {
    {FX_BWSao, FX_BWSxl, FX_BWSxr, FX_BWScn, FX_BWScn, FX_BWSxa, FX_BWSxa,
     FX_BWSao, FX_BWSao, FX_BWSao},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSxr,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSxl,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSao, FX_BWSxl, FX_BWSxr, FX_BWScn, FX_BWScn, FX_BWSxa, FX_BWSao,
     FX_BWSao, FX_BWSao, FX_BWSao},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSro,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlo,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSrt,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlt,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSao, FX_BWSxl, FX_BWSxr, FX_BWScn, FX_BWScn, FX_BWSxa, FX_BWScn,
     FX_BWSac, FX_BWSao, FX_BWSao},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSra,
     FX_BWSrc, FX_BWSro, FX_BWSrt},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSre,
     FX_BWSrs, FX_BWSrs, FX_BWSret},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSla,
     FX_BWSlc, FX_BWSlo, FX_BWSlt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSle,
     FX_BWSls, FX_BWSls, FX_BWSlet},
    {FX_BWSao, FX_BWSxl, FX_BWSxr, FX_BWScn, FX_BWScn, FX_BWSxa, FX_BWSao,
     FX_BWSao, FX_BWSao, FX_BWSao},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSro,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSro,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlo,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlo,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSret,
     FX_BWSro, FX_BWSro, FX_BWSret},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlet,
     FX_BWSlo, FX_BWSlo, FX_BWSlet},
};
const int32_t gc_FX_BidiWeakActions[][10] = {
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxA, FX_BWAxxR,
     FX_BWAxxR, FX_BWAxxN, FX_BWAxxN, FX_BWAxxN},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxR, FX_BWAxxN, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxL, FX_BWAxxN, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxA, FX_BWAxxR,
     FX_BWAxxN, FX_BWAxxN, FX_BWAxxN, FX_BWAxxN},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxN, FX_BWAxxN, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxN, FX_BWAxxN, FX_BWAxxN, FX_BWAxIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAExE, FX_BWANxR,
     FX_BWAxIx, FX_BWANxN, FX_BWANxN, FX_BWAxIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWALxL, FX_BWANxR,
     FX_BWAxIx, FX_BWANxN, FX_BWANxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxA, FX_BWAxxR,
     FX_BWAxxA, FX_BWAxIx, FX_BWAxxN, FX_BWAxxN},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxA, FX_BWAxIx, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxE, FX_BWAxIx, FX_BWAxIx, FX_BWAxxE},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxA, FX_BWAxIx, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxL, FX_BWAxIx, FX_BWAxIx, FX_BWAxxL},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAAxx, FX_BWAAxA, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANxN},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAAxx, FX_BWANxE, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAExE, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAAxx, FX_BWANxL, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWALxL, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxE, FX_BWAxxN, FX_BWAxxN, FX_BWAxxE},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxL, FX_BWAxxN, FX_BWAxxN, FX_BWAxxL},
};
void FX_BidiResolveWeak(int32_t iBaseLevel,
                        CFX_Int32Array& classes,
                        CFX_Int32Array& levels) {
  FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
  FXSYS_assert(classes.GetSize() == levels.GetSize());
  int32_t iSize = classes.GetSize();
  if (iSize < 1) {
    return;
  }
  iSize--;
  int32_t iLevelCur = iBaseLevel;
  int32_t iState = FX_IsOdd(iBaseLevel) ? FX_BWSxr : FX_BWSxl;
  int32_t i = 0, iCount = 0, iClsCur, iClsRun, iClsNew, iAction;
  for (; i <= iSize; i++) {
    iClsCur = classes.GetAt(i);

    FXSYS_assert(iClsCur <= FX_BIDICLASS_BN);
    iAction = gc_FX_BidiWeakActions[iState][iClsCur];
    iClsRun = FX_BidiGetDeferredType(iAction);
    if (iClsRun != FX_BIDIWEAKACTION_XX && iCount > 0) {
      FX_BidiSetDeferredRun(classes, i, iCount, iClsRun);
      iCount = 0;
    }
    iClsNew = FX_BidiGetResolvedType(iAction);
    if (iClsNew != FX_BIDIWEAKACTION_XX) {
      classes.SetAt(i, iClsNew);
    }
    if (FX_BIDIWEAKACTION_IX & iAction) {
      iCount++;
    }
    iState = gc_FX_BidiWeakStates[iState][iClsCur];
  }
  iClsCur = FX_BidiDirection(iLevelCur);
  iClsRun = FX_BidiGetDeferredType(gc_FX_BidiWeakActions[iState][iClsCur]);
  if (iClsRun != FX_BIDIWEAKACTION_XX && iCount > 0) {
    FX_BidiSetDeferredRun(classes, i, iCount, iClsRun);
  }
}
const int32_t gc_FX_BidiNeutralStates[][5] = {
    {FX_BNSrn, FX_BNSl, FX_BNSr, FX_BNSr, FX_BNSr},
    {FX_BNSln, FX_BNSl, FX_BNSr, FX_BNSa, FX_BNSl},
    {FX_BNSrn, FX_BNSl, FX_BNSr, FX_BNSr, FX_BNSr},
    {FX_BNSln, FX_BNSl, FX_BNSr, FX_BNSa, FX_BNSl},
    {FX_BNSna, FX_BNSl, FX_BNSr, FX_BNSa, FX_BNSl},
    {FX_BNSna, FX_BNSl, FX_BNSr, FX_BNSa, FX_BNSl},
};
const int32_t gc_FX_BidiNeutralActions[][5] = {
    {FX_BNAIn, 0, 0, 0, 0},
    {FX_BNAIn, 0, 0, 0, FX_BCL},
    {FX_BNAIn, FX_BNAEn, FX_BNARn, FX_BNARn, FX_BNARn},
    {FX_BNAIn, FX_BNALn, FX_BNAEn, FX_BNAEn, FX_BNALnL},
    {FX_BNAIn, 0, 0, 0, FX_BCL},
    {FX_BNAIn, FX_BNAEn, FX_BNARn, FX_BNARn, FX_BNAEn},
};
int32_t FX_BidiGetDeferredNeutrals(int32_t iAction, int32_t iLevel) {
  iAction = (iAction >> 4) & 0xF;
  if (iAction == (FX_BIDINEUTRALACTION_En >> 4)) {
    return FX_BidiDirection(iLevel);
  } else {
    return iAction;
  }
}
int32_t FX_BidiGetResolvedNeutrals(int32_t iAction) {
  iAction = (iAction & 0xF);
  if (iAction == FX_BIDINEUTRALACTION_In) {
    return 0;
  } else {
    return iAction;
  }
}
void FX_BidiResolveNeutrals(int32_t iBaseLevel,
                            CFX_Int32Array& classes,
                            const CFX_Int32Array& levels) {
  FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
  FXSYS_assert(classes.GetSize() == levels.GetSize());
  int32_t iSize = classes.GetSize();
  if (iSize < 1) {
    return;
  }
  iSize--;
  int32_t iLevel = iBaseLevel;
  int32_t iState = FX_IsOdd(iBaseLevel) ? FX_BNSr : FX_BNSl;
  int32_t i = 0, iCount = 0, iClsCur, iClsRun, iClsNew, iAction;
  for (; i <= iSize; i++) {
    iClsCur = classes.GetAt(i);
    if (iClsCur == FX_BIDICLASS_BN) {
      if (iCount) {
        iCount++;
      }
      continue;
    }
    FXSYS_assert(iClsCur < FX_BIDICLASS_AL);
    iAction = gc_FX_BidiNeutralActions[iState][iClsCur];
    iClsRun = FX_BidiGetDeferredNeutrals(iAction, iLevel);
    if (iClsRun != FX_BIDICLASS_N && iCount > 0) {
      FX_BidiSetDeferredRun(classes, i, iCount, iClsRun);
      iCount = 0;
    }
    iClsNew = FX_BidiGetResolvedNeutrals(iAction);
    if (iClsNew != FX_BIDICLASS_N) {
      classes.SetAt(i, iClsNew);
    }
    if (FX_BIDINEUTRALACTION_In & iAction) {
      iCount++;
    }
    iState = gc_FX_BidiNeutralStates[iState][iClsCur];
    iLevel = levels.GetAt(i);
  }
  iClsCur = FX_BidiDirection(iLevel);
  iClsRun = FX_BidiGetDeferredNeutrals(
      gc_FX_BidiNeutralActions[iState][iClsCur], iLevel);
  if (iClsRun != FX_BIDICLASS_N && iCount > 0) {
    FX_BidiSetDeferredRun(classes, i, iCount, iClsRun);
  }
}
const int32_t gc_FX_BidiAddLevel[][4] = {
    {0, 1, 2, 2},
    {1, 0, 1, 1},
};
void FX_BidiResolveImplicit(const CFX_Int32Array& classes,
                            CFX_Int32Array& levels) {
  FXSYS_assert(classes.GetSize() == levels.GetSize());
  int32_t iSize = classes.GetSize();
  if (iSize < 1) {
    return;
  }
  iSize--;
  int32_t iCls, iLevel;
  for (int32_t i = 0; i <= iSize; i++) {
    iCls = classes.GetAt(i);
    if (iCls == FX_BIDICLASS_BN) {
      continue;
    }
    FXSYS_assert(iCls > FX_BIDICLASS_ON && iCls < FX_BIDICLASS_AL);
    iLevel = levels.GetAt(i);
    iLevel += gc_FX_BidiAddLevel[FX_IsOdd(iLevel)][iCls - 1];
    levels.SetAt(i, iLevel);
  }
}
void FX_BidiResolveWhitespace(int32_t iBaseLevel,
                              const CFX_Int32Array& classes,
                              CFX_Int32Array& levels) {
  FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
  FXSYS_assert(classes.GetSize() == levels.GetSize());
  int32_t iSize = classes.GetSize();
  if (iSize < 1) {
    return;
  }
  iSize--;
  int32_t iLevel = iBaseLevel;
  int32_t i = 0, iCount = 0;
  for (; i <= iSize; i++) {
    switch (classes.GetAt(i)) {
      case FX_BIDICLASS_WS:
        iCount++;
        break;
      case FX_BIDICLASS_RLE:
      case FX_BIDICLASS_LRE:
      case FX_BIDICLASS_LRO:
      case FX_BIDICLASS_RLO:
      case FX_BIDICLASS_PDF:
      case FX_BIDICLASS_BN:
        levels.SetAt(i, iLevel);
        iCount++;
        break;
      case FX_BIDICLASS_S:
      case FX_BIDICLASS_B:
        if (iCount > 0) {
          FX_BidiSetDeferredRun(levels, i, iCount, iBaseLevel);
        }
        levels.SetAt(i, iBaseLevel);
        iCount = 0;
        break;
      default:
        iCount = 0;
        break;
    }
    iLevel = levels.GetAt(i);
  }
  if (iCount > 0) {
    FX_BidiSetDeferredRun(levels, i, iCount, iBaseLevel);
  }
}
int32_t FX_BidiReorderLevel(int32_t iBaseLevel,
                            CFX_WideString& wsText,
                            const CFX_Int32Array& levels,
                            int32_t iStart,
                            FX_BOOL bReverse) {
  FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
  FXSYS_assert(wsText.GetLength() == levels.GetSize());
  FXSYS_assert(iStart >= 0 && iStart < wsText.GetLength());
  int32_t iSize = wsText.GetLength();
  if (iSize < 1) {
    return 0;
  }
  bReverse = bReverse || FX_IsOdd(iBaseLevel);
  int32_t i = iStart, iLevel;
  for (; i < iSize; i++) {
    if ((iLevel = levels.GetAt(i)) == iBaseLevel) {
      continue;
    }
    if (iLevel < iBaseLevel) {
      break;
    }
    i += FX_BidiReorderLevel(iBaseLevel + 1, wsText, levels, i, bReverse) - 1;
  }
  int32_t iCount = i - iStart;
  if (bReverse && iCount > 1) {
    FX_BidiReverseString(wsText, iStart, iCount);
  }
  return iCount;
}
void FX_BidiReorder(int32_t iBaseLevel,
                    CFX_WideString& wsText,
                    const CFX_Int32Array& levels) {
  FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
  FXSYS_assert(wsText.GetLength() == levels.GetSize());
  int32_t iSize = wsText.GetLength();
  if (iSize < 1) {
    return;
  }
  int32_t i = 0;
  while (i < iSize) {
    i += FX_BidiReorderLevel(iBaseLevel, wsText, levels, i, FALSE);
  }
}
void FX_BidiLine(CFX_WideString& wsText, int32_t iBaseLevel) {
  int32_t iLength = wsText.GetLength();
  if (iLength < 2) {
    return;
  }
  CFX_Int32Array classes, levels;
  classes.SetAtGrow(iLength - 1, 0);
  levels.SetAtGrow(iLength - 1, 0);
  FX_BidiClassify(wsText, classes, FALSE);
  FX_BidiResolveExplicit(iBaseLevel, FX_BIDICLASS_N, classes, levels, 0,
                         iLength, 0);
  FX_BidiResolveWeak(iBaseLevel, classes, levels);
  FX_BidiResolveNeutrals(iBaseLevel, classes, levels);
  FX_BidiResolveImplicit(classes, levels);
  FX_BidiClassify(wsText, classes, TRUE);
  FX_BidiResolveWhitespace(iBaseLevel, classes, levels);
  FX_BidiReorder(iBaseLevel, wsText, levels);
  classes.RemoveAll();
  levels.RemoveAll();
}
template <class baseType>
class CFX_BidiLineTemplate {
 public:
  void FX_BidiReverseString(CFX_ArrayTemplate<baseType>& chars,
                            int32_t iStart,
                            int32_t iCount) {
    FXSYS_assert(iStart > -1 && iStart < chars.GetSize());
    FXSYS_assert(iCount >= 0 && iStart + iCount <= chars.GetSize());
    baseType *pStart, *pEnd;
    int32_t iEnd = iStart + iCount - 1, iTemp;
    while (iStart < iEnd) {
      pStart = chars.GetDataPtr(iStart++);
      pEnd = chars.GetDataPtr(iEnd--);
      iTemp = pStart->m_iBidiPos;
      pStart->m_iBidiPos = pEnd->m_iBidiPos;
      pEnd->m_iBidiPos = iTemp;
    }
  }
  void FX_BidiSetDeferredRun(CFX_ArrayTemplate<baseType>& chars,
                             FX_BOOL bClass,
                             int32_t iStart,
                             int32_t iCount,
                             int32_t iValue) {
    FXSYS_assert(iStart > -1 && iStart <= chars.GetSize());
    FXSYS_assert(iStart - iCount > -1);
    baseType* pTC;
    int32_t iLast = iStart - iCount;
    if (bClass) {
      for (int32_t i = iStart - 1; i >= iLast; i--) {
        pTC = chars.GetDataPtr(i);
        pTC->m_iBidiClass = (int16_t)iValue;
      }
    } else {
      for (int32_t i = iStart - 1; i >= iLast; i--) {
        pTC = chars.GetDataPtr(i);
        pTC->m_iBidiLevel = (int16_t)iValue;
      }
    }
  }
  void FX_BidiClassify(CFX_ArrayTemplate<baseType>& chars,
                       int32_t iCount,
                       FX_BOOL bWS) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    baseType* pTC;
    if (bWS) {
      for (int32_t i = 0; i < iCount; i++) {
        pTC = chars.GetDataPtr(i);
        pTC->m_iBidiClass =
            (int16_t)(pTC->m_dwCharProps & FX_BIDICLASSBITSMASK) >>
            FX_BIDICLASSBITS;
      }
    } else {
      for (int32_t i = 0; i < iCount; i++) {
        pTC = chars.GetDataPtr(i);
        pTC->m_iBidiClass = (int16_t)
            gc_FX_BidiNTypes[(pTC->m_dwCharProps & FX_BIDICLASSBITSMASK) >>
                             FX_BIDICLASSBITS];
      }
    }
  }
  void FX_BidiResolveExplicit(CFX_ArrayTemplate<baseType>& chars,
                              int32_t iCount,
                              int32_t iBaseLevel) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
    if (iCount < 1) {
      return;
    }
    baseType* pTC;
    for (int32_t i = 0; i < iCount; i++) {
      pTC = chars.GetDataPtr(i);
      pTC->m_iBidiLevel = (int16_t)iBaseLevel;
    }
  }
  void FX_BidiResolveWeak(CFX_ArrayTemplate<baseType>& chars,
                          int32_t iCount,
                          int32_t iBaseLevel) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    iCount--;
    if (iCount < 1) {
      return;
    }
    baseType *pTC, *pTCNext;
    int32_t iLevelCur = iBaseLevel;
    int32_t iState = FX_IsOdd(iBaseLevel) ? FX_BWSxr : FX_BWSxl;
    int32_t i = 0, iNum = 0, iClsCur, iClsRun, iClsNew, iAction;
    for (; i <= iCount; i++) {
      pTC = chars.GetDataPtr(i);
      iClsCur = pTC->m_iBidiClass;
      if (iClsCur == FX_BIDICLASS_BN) {
        pTC->m_iBidiLevel = (int16_t)iLevelCur;
        if (i == iCount && iLevelCur != iBaseLevel) {
          iClsCur = FX_BidiDirection(iLevelCur);
          pTC->m_iBidiClass = (int16_t)iClsCur;
        } else if (i < iCount) {
          pTCNext = chars.GetDataPtr(i + 1);
          int32_t iLevelNext, iLevelNew;
          iClsNew = pTCNext->m_iBidiClass;
          iLevelNext = pTCNext->m_iBidiLevel;
          if (iClsNew != FX_BIDICLASS_BN && iLevelCur != iLevelNext) {
            iLevelNew = iLevelNext;
            if (iLevelCur > iLevelNew) {
              iLevelNew = iLevelCur;
            }
            pTC->m_iBidiLevel = (int16_t)iLevelNew;
            iClsCur = FX_BidiDirection(iLevelNew);
            pTC->m_iBidiClass = (int16_t)iClsCur;
            iLevelCur = iLevelNext;
          } else {
            if (iNum > 0) {
              iNum++;
            }
            continue;
          }
        } else {
          if (iNum > 0) {
            iNum++;
          }
          continue;
        }
      }
      FXSYS_assert(iClsCur <= FX_BIDICLASS_BN);
      iAction = gc_FX_BidiWeakActions[iState][iClsCur];
      iClsRun = FX_BidiGetDeferredType(iAction);
      if (iClsRun != FX_BIDIWEAKACTION_XX && iNum > 0) {
        FX_BidiSetDeferredRun(chars, TRUE, i, iNum, iClsRun);
        iNum = 0;
      }
      iClsNew = FX_BidiGetResolvedType(iAction);
      if (iClsNew != FX_BIDIWEAKACTION_XX) {
        pTC->m_iBidiClass = (int16_t)iClsNew;
      }
      if (FX_BIDIWEAKACTION_IX & iAction) {
        iNum++;
      }
      iState = gc_FX_BidiWeakStates[iState][iClsCur];
    }
    if (iNum > 0) {
      iClsCur = FX_BidiDirection(iBaseLevel);
      iClsRun = FX_BidiGetDeferredType(gc_FX_BidiWeakActions[iState][iClsCur]);
      if (iClsRun != FX_BIDIWEAKACTION_XX) {
        FX_BidiSetDeferredRun(chars, TRUE, i, iNum, iClsRun);
      }
    }
  }
  void FX_BidiResolveNeutrals(CFX_ArrayTemplate<baseType>& chars,
                              int32_t iCount,
                              int32_t iBaseLevel) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
    iCount--;
    if (iCount < 1) {
      return;
    }
    baseType* pTC;
    int32_t iLevel = iBaseLevel;
    int32_t iState = FX_IsOdd(iBaseLevel) ? FX_BNSr : FX_BNSl;
    int32_t i = 0, iNum = 0, iClsCur, iClsRun, iClsNew, iAction;
    for (; i <= iCount; i++) {
      pTC = chars.GetDataPtr(i);
      iClsCur = pTC->m_iBidiClass;
      if (iClsCur == FX_BIDICLASS_BN) {
        if (iNum) {
          iNum++;
        }
        continue;
      }
      FXSYS_assert(iClsCur < FX_BIDICLASS_AL);
      iAction = gc_FX_BidiNeutralActions[iState][iClsCur];
      iClsRun = FX_BidiGetDeferredNeutrals(iAction, iLevel);
      if (iClsRun != FX_BIDICLASS_N && iNum > 0) {
        FX_BidiSetDeferredRun(chars, TRUE, i, iNum, iClsRun);
        iNum = 0;
      }
      iClsNew = FX_BidiGetResolvedNeutrals(iAction);
      if (iClsNew != FX_BIDICLASS_N) {
        pTC->m_iBidiClass = (int16_t)iClsNew;
      }
      if (FX_BIDINEUTRALACTION_In & iAction) {
        iNum++;
      }
      iState = gc_FX_BidiNeutralStates[iState][iClsCur];
      iLevel = pTC->m_iBidiLevel;
    }
    if (iNum > 0) {
      iClsCur = FX_BidiDirection(iLevel);
      iClsRun = FX_BidiGetDeferredNeutrals(
          gc_FX_BidiNeutralActions[iState][iClsCur], iLevel);
      if (iClsRun != FX_BIDICLASS_N) {
        FX_BidiSetDeferredRun(chars, TRUE, i, iNum, iClsRun);
      }
    }
  }
  void FX_BidiResolveImplicit(CFX_ArrayTemplate<baseType>& chars,
                              int32_t iCount) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    baseType* pTC;
    int32_t iCls, iLevel;
    for (int32_t i = 0; i < iCount; i++) {
      pTC = chars.GetDataPtr(i);
      iCls = pTC->m_iBidiClass;
      if (iCls == FX_BIDICLASS_BN) {
        continue;
      }
      FXSYS_assert(iCls > FX_BIDICLASS_ON && iCls < FX_BIDICLASS_AL);
      iLevel = pTC->m_iBidiLevel;
      iLevel += gc_FX_BidiAddLevel[FX_IsOdd(iLevel)][iCls - 1];
      pTC->m_iBidiLevel = (int16_t)iLevel;
    }
  }
  void FX_BidiResolveWhitespace(CFX_ArrayTemplate<baseType>& chars,
                                int32_t iCount,
                                int32_t iBaseLevel) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
    if (iCount < 1) {
      return;
    }
    iCount--;
    int32_t iLevel = iBaseLevel;
    int32_t i = 0, iNum = 0;
    baseType* pTC;
    for (; i <= iCount; i++) {
      pTC = chars.GetDataPtr(i);
      switch (pTC->m_iBidiClass) {
        case FX_BIDICLASS_WS:
          iNum++;
          break;
        case FX_BIDICLASS_RLE:
        case FX_BIDICLASS_LRE:
        case FX_BIDICLASS_LRO:
        case FX_BIDICLASS_RLO:
        case FX_BIDICLASS_PDF:
        case FX_BIDICLASS_BN:
          pTC->m_iBidiLevel = (int16_t)iLevel;
          iNum++;
          break;
        case FX_BIDICLASS_S:
        case FX_BIDICLASS_B:
          if (iNum > 0) {
            FX_BidiSetDeferredRun(chars, FALSE, i, iNum, iBaseLevel);
          }
          pTC->m_iBidiLevel = (int16_t)iBaseLevel;
          iNum = 0;
          break;
        default:
          iNum = 0;
          break;
      }
      iLevel = pTC->m_iBidiLevel;
    }
    if (iNum > 0) {
      FX_BidiSetDeferredRun(chars, FALSE, i, iNum, iBaseLevel);
    }
  }
  int32_t FX_BidiReorderLevel(CFX_ArrayTemplate<baseType>& chars,
                              int32_t iCount,
                              int32_t iBaseLevel,
                              int32_t iStart,
                              FX_BOOL bReverse) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
    FXSYS_assert(iStart >= 0 && iStart < iCount);
    if (iCount < 1) {
      return 0;
    }
    baseType* pTC;
    bReverse = bReverse || FX_IsOdd(iBaseLevel);
    int32_t i = iStart, iLevel;
    for (; i < iCount; i++) {
      pTC = chars.GetDataPtr(i);
      if ((iLevel = pTC->m_iBidiLevel) == iBaseLevel) {
        continue;
      }
      if (iLevel < iBaseLevel) {
        break;
      }
      i += FX_BidiReorderLevel(chars, iCount, iBaseLevel + 1, i, bReverse) - 1;
    }
    int32_t iNum = i - iStart;
    if (bReverse && iNum > 1) {
      FX_BidiReverseString(chars, iStart, iNum);
    }
    return iNum;
  }
  void FX_BidiReorder(CFX_ArrayTemplate<baseType>& chars,
                      int32_t iCount,
                      int32_t iBaseLevel) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    FXSYS_assert(iBaseLevel >= 0 && iBaseLevel <= FX_BIDIMAXLEVEL);
    int32_t i = 0;
    while (i < iCount) {
      i += FX_BidiReorderLevel(chars, iCount, iBaseLevel, i, FALSE);
    }
  }
  void FX_BidiPosition(CFX_ArrayTemplate<baseType>& chars, int32_t iCount) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    baseType* pTC;
    int32_t i = 0;
    while (i < iCount) {
      pTC = chars.GetDataPtr(i);
      pTC = chars.GetDataPtr(pTC->m_iBidiPos);
      pTC->m_iBidiOrder = i++;
    }
  }

  void FX_BidiLine(CFX_ArrayTemplate<baseType>& chars,
                   int32_t iCount,
                   int32_t iBaseLevel) {
    FXSYS_assert(iCount > -1 && iCount <= chars.GetSize());
    if (iCount < 2) {
      return;
    }
    FX_BidiClassify(chars, iCount, FALSE);
    FX_BidiResolveExplicit(chars, iCount, iBaseLevel);
    FX_BidiResolveWeak(chars, iCount, iBaseLevel);
    FX_BidiResolveNeutrals(chars, iCount, iBaseLevel);
    FX_BidiResolveImplicit(chars, iCount);
    FX_BidiClassify(chars, iCount, TRUE);
    FX_BidiResolveWhitespace(chars, iCount, iBaseLevel);
    FX_BidiReorder(chars, iCount, iBaseLevel);
    FX_BidiPosition(chars, iCount);
  }
};
void FX_BidiLine(CFX_TxtCharArray& chars, int32_t iCount, int32_t iBaseLevel) {
  CFX_BidiLineTemplate<CFX_TxtChar> blt;
  blt.FX_BidiLine(chars, iCount, iBaseLevel);
}
void FX_BidiLine(CFX_RTFCharArray& chars, int32_t iCount, int32_t iBaseLevel) {
  CFX_BidiLineTemplate<CFX_RTFChar> blt;
  blt.FX_BidiLine(chars, iCount, iBaseLevel);
}

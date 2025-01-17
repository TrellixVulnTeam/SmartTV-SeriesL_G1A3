// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_font/font_int.h"

#include "core/fpdfapi/fpdf_cmaps/cmap_int.h"
#include "core/fpdfapi/fpdf_font/ttgsubtable.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_array.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_dictionary.h"
#include "core/fpdfapi/fpdf_parser/include/cpdf_simple_parser.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"
#include "core/fxcrt/include/fx_ext.h"
#include "core/include/fxge/fx_freetype.h"
#include "core/include/fxge/fx_ge.h"

namespace {

const FX_CHAR* const g_CharsetNames[CIDSET_NUM_SETS] = {
    nullptr, "GB1", "CNS1", "Japan1", "Korea1", "UCS"};


class CPDF_PredefinedCMap {
 public:
  const FX_CHAR* m_pName;
  CIDSet m_Charset;
  CIDCoding m_Coding;
  CPDF_CMap::CodingScheme m_CodingScheme;
  uint8_t m_LeadingSegCount;
  uint8_t m_LeadingSegs[4];
};

const CPDF_PredefinedCMap g_PredefinedCMaps[] = {
    {"GB-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0xa1, 0xfe}},
    {"GBpc-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0xa1, 0xfc}},
    {"GBK-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0x81, 0xfe}},
    {"GBKp-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0x81, 0xfe}},
    {"GBK2K-EUC",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0x81, 0xfe}},
    {"GBK2K",
     CIDSET_GB1,
     CIDCODING_GB,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0x81, 0xfe}},
    {"UniGB-UCS2", CIDSET_GB1, CIDCODING_UCS2, CPDF_CMap::TwoBytes, 0, {}},
    {"UniGB-UTF16", CIDSET_GB1, CIDCODING_UTF16, CPDF_CMap::TwoBytes, 0, {}},
    {"B5pc",
     CIDSET_CNS1,
     CIDCODING_BIG5,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0xa1, 0xfc}},
    {"HKscs-B5",
     CIDSET_CNS1,
     CIDCODING_BIG5,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0x88, 0xfe}},
    {"ETen-B5",
     CIDSET_CNS1,
     CIDCODING_BIG5,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0xa1, 0xfe}},
    {"ETenms-B5",
     CIDSET_CNS1,
     CIDCODING_BIG5,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0xa1, 0xfe}},
    {"UniCNS-UCS2", CIDSET_CNS1, CIDCODING_UCS2, CPDF_CMap::TwoBytes, 0, {}},
    {"UniCNS-UTF16", CIDSET_CNS1, CIDCODING_UTF16, CPDF_CMap::TwoBytes, 0, {}},
    {"83pv-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {0x81, 0x9f, 0xe0, 0xfc}},
    {"90ms-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {0x81, 0x9f, 0xe0, 0xfc}},
    {"90msp-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {0x81, 0x9f, 0xe0, 0xfc}},
    {"90pv-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {0x81, 0x9f, 0xe0, 0xfc}},
    {"Add-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {0x81, 0x9f, 0xe0, 0xfc}},
    {"EUC",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {0x8e, 0x8e, 0xa1, 0xfe}},
    {"H", CIDSET_JAPAN1, CIDCODING_JIS, CPDF_CMap::TwoBytes, 1, {0x21, 0x7e}},
    {"V", CIDSET_JAPAN1, CIDCODING_JIS, CPDF_CMap::TwoBytes, 1, {0x21, 0x7e}},
    {"Ext-RKSJ",
     CIDSET_JAPAN1,
     CIDCODING_JIS,
     CPDF_CMap::MixedTwoBytes,
     2,
     {0x81, 0x9f, 0xe0, 0xfc}},
    {"UniJIS-UCS2", CIDSET_JAPAN1, CIDCODING_UCS2, CPDF_CMap::TwoBytes, 0, {}},
    {"UniJIS-UCS2-HW",
     CIDSET_JAPAN1,
     CIDCODING_UCS2,
     CPDF_CMap::TwoBytes,
     0,
     {}},
    {"UniJIS-UTF16",
     CIDSET_JAPAN1,
     CIDCODING_UTF16,
     CPDF_CMap::TwoBytes,
     0,
     {}},
    {"KSC-EUC",
     CIDSET_KOREA1,
     CIDCODING_KOREA,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0xa1, 0xfe}},
    {"KSCms-UHC",
     CIDSET_KOREA1,
     CIDCODING_KOREA,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0x81, 0xfe}},
    {"KSCms-UHC-HW",
     CIDSET_KOREA1,
     CIDCODING_KOREA,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0x81, 0xfe}},
    {"KSCpc-EUC",
     CIDSET_KOREA1,
     CIDCODING_KOREA,
     CPDF_CMap::MixedTwoBytes,
     1,
     {0xa1, 0xfd}},
    {"UniKS-UCS2", CIDSET_KOREA1, CIDCODING_UCS2, CPDF_CMap::TwoBytes, 0, {}},
    {"UniKS-UTF16", CIDSET_KOREA1, CIDCODING_UTF16, CPDF_CMap::TwoBytes, 0, {}},
};

CIDSet CIDSetFromSizeT(size_t index) {
  if (index >= CIDSET_NUM_SETS) {
    NOTREACHED();
    return CIDSET_UNKNOWN;
  }
  return static_cast<CIDSet>(index);
}

CFX_ByteString CMap_GetString(const CFX_ByteStringC& word) {
  return word.Mid(1, word.GetLength() - 2);
}

int CompareDWORD(const void* data1, const void* data2) {
  return (*(uint32_t*)data1) - (*(uint32_t*)data2);
}

int CompareCID(const void* key, const void* element) {
  if ((*(uint32_t*)key) < (*(uint32_t*)element)) {
    return -1;
  }
  if ((*(uint32_t*)key) >
      (*(uint32_t*)element) + ((uint32_t*)element)[1] / 65536) {
    return 1;
  }
  return 0;
}

int CheckCodeRange(uint8_t* codes,
                   int size,
                   CMap_CodeRange* pRanges,
                   int nRanges) {
  int iSeg = nRanges - 1;
  while (iSeg >= 0) {
    if (pRanges[iSeg].m_CharSize < size) {
      --iSeg;
      continue;
    }
    int iChar = 0;
    while (iChar < size) {
      if (codes[iChar] < pRanges[iSeg].m_Lower[iChar] ||
          codes[iChar] > pRanges[iSeg].m_Upper[iChar]) {
        break;
      }
      ++iChar;
    }
    if (iChar == pRanges[iSeg].m_CharSize)
      return 2;

    if (iChar)
      return (size == pRanges[iSeg].m_CharSize) ? 2 : 1;
    iSeg--;
  }
  return 0;
}

int GetCharSizeImpl(uint32_t charcode,
                    CMap_CodeRange* pRanges,
                    int iRangesSize) {
  if (!iRangesSize)
    return 1;

  uint8_t codes[4];
  codes[0] = codes[1] = 0x00;
  codes[2] = (uint8_t)(charcode >> 8 & 0xFF);
  codes[3] = (uint8_t)charcode;
  int offset = 0;
  int size = 4;
  for (int i = 0; i < 4; ++i) {
    int iSeg = iRangesSize - 1;
    while (iSeg >= 0) {
      if (pRanges[iSeg].m_CharSize < size) {
        --iSeg;
        continue;
      }
      int iChar = 0;
      while (iChar < size) {
        if (codes[offset + iChar] < pRanges[iSeg].m_Lower[iChar] ||
            codes[offset + iChar] > pRanges[iSeg].m_Upper[iChar]) {
          break;
        }
        ++iChar;
      }
      if (iChar == pRanges[iSeg].m_CharSize)
        return size;
      --iSeg;
    }
    --size;
    ++offset;
  }
  return 1;
}

}  // namespace

CPDF_CMapManager::CPDF_CMapManager() {
  m_bPrompted = FALSE;
  FXSYS_memset(m_CID2UnicodeMaps, 0, sizeof m_CID2UnicodeMaps);
}
CPDF_CMapManager::~CPDF_CMapManager() {
  for (const auto& pair : m_CMaps) {
    delete pair.second;
  }
  m_CMaps.clear();
  for (size_t i = 0; i < FX_ArraySize(m_CID2UnicodeMaps); ++i) {
    delete m_CID2UnicodeMaps[i];
  }
}
CPDF_CMap* CPDF_CMapManager::GetPredefinedCMap(const CFX_ByteString& name,
                                               FX_BOOL bPromptCJK) {
  auto it = m_CMaps.find(name);
  if (it != m_CMaps.end()) {
    return it->second;
  }
  CPDF_CMap* pCMap = LoadPredefinedCMap(name, bPromptCJK);
  if (!name.IsEmpty()) {
    m_CMaps[name] = pCMap;
  }
  return pCMap;
}
CPDF_CMap* CPDF_CMapManager::LoadPredefinedCMap(const CFX_ByteString& name,
                                                FX_BOOL bPromptCJK) {
  CPDF_CMap* pCMap = new CPDF_CMap;
  const FX_CHAR* pname = name;
  if (*pname == '/') {
    pname++;
  }
  pCMap->LoadPredefined(this, pname, bPromptCJK);
  return pCMap;
}

void CPDF_CMapManager::ReloadAll() {
  for (const auto& pair : m_CMaps) {
    CPDF_CMap* pCMap = pair.second;
    pCMap->LoadPredefined(this, pair.first, FALSE);
  }
  for (size_t i = 0; i < FX_ArraySize(m_CID2UnicodeMaps); ++i) {
    if (CPDF_CID2UnicodeMap* pMap = m_CID2UnicodeMaps[i]) {
      pMap->Load(this, CIDSetFromSizeT(i), FALSE);
    }
  }
}
CPDF_CID2UnicodeMap* CPDF_CMapManager::GetCID2UnicodeMap(CIDSet charset,
                                                         FX_BOOL bPromptCJK) {
  if (!m_CID2UnicodeMaps[charset])
    m_CID2UnicodeMaps[charset] = LoadCID2UnicodeMap(charset, bPromptCJK);
  return m_CID2UnicodeMaps[charset];
}
CPDF_CID2UnicodeMap* CPDF_CMapManager::LoadCID2UnicodeMap(CIDSet charset,
                                                          FX_BOOL bPromptCJK) {
  CPDF_CID2UnicodeMap* pMap = new CPDF_CID2UnicodeMap();
  if (!pMap->Initialize()) {
    delete pMap;
    return NULL;
  }
  pMap->Load(this, charset, bPromptCJK);
  return pMap;
}
CPDF_CMapParser::CPDF_CMapParser() {
  m_pCMap = NULL;
  m_Status = 0;
  m_CodeSeq = 0;
}
FX_BOOL CPDF_CMapParser::Initialize(CPDF_CMap* pCMap) {
  m_pCMap = pCMap;
  m_Status = 0;
  m_CodeSeq = 0;
  m_AddMaps.EstimateSize(0, 10240);
  return TRUE;
}

void CPDF_CMapParser::ParseWord(const CFX_ByteStringC& word) {
  if (word.IsEmpty()) {
    return;
  }
  if (word == "begincidchar") {
    m_Status = 1;
    m_CodeSeq = 0;
  } else if (word == "begincidrange") {
    m_Status = 2;
    m_CodeSeq = 0;
  } else if (word == "endcidrange" || word == "endcidchar") {
    m_Status = 0;
  } else if (word == "/WMode") {
    m_Status = 6;
  } else if (word == "/Registry") {
    m_Status = 3;
  } else if (word == "/Ordering") {
    m_Status = 4;
  } else if (word == "/Supplement") {
    m_Status = 5;
  } else if (word == "begincodespacerange") {
    m_Status = 7;
    m_CodeSeq = 0;
  } else if (word == "usecmap") {
  } else if (m_Status == 1 || m_Status == 2) {
    m_CodePoints[m_CodeSeq] = CMap_GetCode(word);
    m_CodeSeq++;
    uint32_t StartCode, EndCode;
    uint16_t StartCID;
    if (m_Status == 1) {
      if (m_CodeSeq < 2) {
        return;
      }
      EndCode = StartCode = m_CodePoints[0];
      StartCID = (uint16_t)m_CodePoints[1];
    } else {
      if (m_CodeSeq < 3) {
        return;
      }
      StartCode = m_CodePoints[0];
      EndCode = m_CodePoints[1];
      StartCID = (uint16_t)m_CodePoints[2];
    }
    if (EndCode < 0x10000) {
      for (uint32_t code = StartCode; code <= EndCode; code++) {
        m_pCMap->m_pMapping[code] = (uint16_t)(StartCID + code - StartCode);
      }
    } else {
      uint32_t buf[2];
      buf[0] = StartCode;
      buf[1] = ((EndCode - StartCode) << 16) + StartCID;
      m_AddMaps.AppendBlock(buf, sizeof buf);
    }
    m_CodeSeq = 0;
  } else if (m_Status == 3) {
    CMap_GetString(word);
    m_Status = 0;
  } else if (m_Status == 4) {
    m_pCMap->m_Charset = CharsetFromOrdering(CMap_GetString(word));
    m_Status = 0;
  } else if (m_Status == 5) {
    CMap_GetCode(word);
    m_Status = 0;
  } else if (m_Status == 6) {
    m_pCMap->m_bVertical = CMap_GetCode(word);
    m_Status = 0;
  } else if (m_Status == 7) {
    if (word == "endcodespacerange") {
      int nSegs = m_CodeRanges.GetSize();
      if (nSegs > 1) {
        m_pCMap->m_CodingScheme = CPDF_CMap::MixedFourBytes;
        m_pCMap->m_nCodeRanges = nSegs;
        m_pCMap->m_pLeadingBytes =
            FX_Alloc2D(uint8_t, nSegs, sizeof(CMap_CodeRange));
        FXSYS_memcpy(m_pCMap->m_pLeadingBytes, m_CodeRanges.GetData(),
                     nSegs * sizeof(CMap_CodeRange));
      } else if (nSegs == 1) {
        m_pCMap->m_CodingScheme = (m_CodeRanges[0].m_CharSize == 2)
                                      ? CPDF_CMap::TwoBytes
                                      : CPDF_CMap::OneByte;
      }
      m_Status = 0;
    } else {
      if (word.GetLength() == 0 || word.GetAt(0) != '<') {
        return;
      }
      if (m_CodeSeq % 2) {
        CMap_CodeRange range;
        if (CMap_GetCodeRange(range, m_LastWord.AsByteStringC(), word)) {
          m_CodeRanges.Add(range);
        }
      }
      m_CodeSeq++;
    }
  }
  m_LastWord = word;
}

// Static.
uint32_t CPDF_CMapParser::CMap_GetCode(const CFX_ByteStringC& word) {
  int num = 0;
  if (word.GetAt(0) == '<') {
    for (int i = 1; i < word.GetLength() && std::isxdigit(word.GetAt(i)); ++i)
      num = num * 16 + FXSYS_toHexDigit(word.GetAt(i));
    return num;
  }

  for (int i = 0; i < word.GetLength() && std::isdigit(word.GetAt(i)); ++i)
    num = num * 10 + FXSYS_toDecimalDigit(static_cast<FX_WCHAR>(word.GetAt(i)));
  return num;
}

// Static.
bool CPDF_CMapParser::CMap_GetCodeRange(CMap_CodeRange& range,
                                        const CFX_ByteStringC& first,
                                        const CFX_ByteStringC& second) {
  if (first.GetLength() == 0 || first.GetAt(0) != '<')
    return false;

  int i;
  for (i = 1; i < first.GetLength(); ++i) {
    if (first.GetAt(i) == '>') {
      break;
    }
  }
  range.m_CharSize = (i - 1) / 2;
  if (range.m_CharSize > 4)
    return false;

  for (i = 0; i < range.m_CharSize; ++i) {
    uint8_t digit1 = first.GetAt(i * 2 + 1);
    uint8_t digit2 = first.GetAt(i * 2 + 2);
    range.m_Lower[i] = FXSYS_toHexDigit(digit1) * 16 + FXSYS_toHexDigit(digit2);
  }

  uint32_t size = second.GetLength();
  for (i = 0; i < range.m_CharSize; ++i) {
    uint8_t digit1 = ((uint32_t)i * 2 + 1 < size)
                         ? second.GetAt((FX_STRSIZE)i * 2 + 1)
                         : '0';
    uint8_t digit2 = ((uint32_t)i * 2 + 2 < size)
                         ? second.GetAt((FX_STRSIZE)i * 2 + 2)
                         : '0';
    range.m_Upper[i] = FXSYS_toHexDigit(digit1) * 16 + FXSYS_toHexDigit(digit2);
  }
  return true;
}

CPDF_CMap::CPDF_CMap() {
  m_Charset = CIDSET_UNKNOWN;
  m_Coding = CIDCODING_UNKNOWN;
  m_CodingScheme = TwoBytes;
  m_bVertical = 0;
  m_bLoaded = FALSE;
  m_pMapping = NULL;
  m_pLeadingBytes = NULL;
  m_pAddMapping = NULL;
  m_pEmbedMap = NULL;
  m_pUseMap = NULL;
  m_nCodeRanges = 0;
}
CPDF_CMap::~CPDF_CMap() {
  FX_Free(m_pMapping);
  FX_Free(m_pAddMapping);
  FX_Free(m_pLeadingBytes);
  delete m_pUseMap;
}
void CPDF_CMap::Release() {
  if (m_PredefinedCMap.IsEmpty()) {
    delete this;
  }
}

FX_BOOL CPDF_CMap::LoadPredefined(CPDF_CMapManager* pMgr,
                                  const FX_CHAR* pName,
                                  FX_BOOL bPromptCJK) {
  m_PredefinedCMap = pName;
  if (m_PredefinedCMap == "Identity-H" || m_PredefinedCMap == "Identity-V") {
    m_Coding = CIDCODING_CID;
    m_bVertical = pName[9] == 'V';
    m_bLoaded = TRUE;
    return TRUE;
  }
  CFX_ByteString cmapid = m_PredefinedCMap;
  m_bVertical = cmapid.Right(1) == "V";
  if (cmapid.GetLength() > 2) {
    cmapid = cmapid.Left(cmapid.GetLength() - 2);
  }
  const CPDF_PredefinedCMap* map = nullptr;
  for (size_t i = 0; i < FX_ArraySize(g_PredefinedCMaps); ++i) {
    if (cmapid == CFX_ByteStringC(g_PredefinedCMaps[i].m_pName)) {
      map = &g_PredefinedCMaps[i];
      break;
    }
  }
  if (!map)
    return FALSE;

  m_Charset = map->m_Charset;
  m_Coding = map->m_Coding;
  m_CodingScheme = map->m_CodingScheme;
  if (m_CodingScheme == MixedTwoBytes) {
    m_pLeadingBytes = FX_Alloc(uint8_t, 256);
    for (uint32_t i = 0; i < map->m_LeadingSegCount; ++i) {
      const uint8_t* segs = map->m_LeadingSegs;
      for (int b = segs[i * 2]; b <= segs[i * 2 + 1]; ++b) {
        m_pLeadingBytes[b] = 1;
      }
    }
  }
  FPDFAPI_FindEmbeddedCMap(pName, m_Charset, m_Coding, m_pEmbedMap);
  if (m_pEmbedMap) {
    m_bLoaded = TRUE;
    return TRUE;
  }
  return FALSE;
}
FX_BOOL CPDF_CMap::LoadEmbedded(const uint8_t* pData, uint32_t size) {
  m_pMapping = FX_Alloc(uint16_t, 65536);
  CPDF_CMapParser parser;
  parser.Initialize(this);
  CPDF_SimpleParser syntax(pData, size);
  while (1) {
    CFX_ByteStringC word = syntax.GetWord();
    if (word.IsEmpty()) {
      break;
    }
    parser.ParseWord(word);
  }
  if (m_CodingScheme == MixedFourBytes && parser.m_AddMaps.GetSize()) {
    m_pAddMapping = FX_Alloc(uint8_t, parser.m_AddMaps.GetSize() + 4);
    *(uint32_t*)m_pAddMapping = parser.m_AddMaps.GetSize() / 8;
    FXSYS_memcpy(m_pAddMapping + 4, parser.m_AddMaps.GetBuffer(),
                 parser.m_AddMaps.GetSize());
    FXSYS_qsort(m_pAddMapping + 4, parser.m_AddMaps.GetSize() / 8, 8,
                CompareDWORD);
  }
  return TRUE;
}

uint16_t CPDF_CMap::CIDFromCharCode(uint32_t charcode) const {
  if (m_Coding == CIDCODING_CID) {
    return (uint16_t)charcode;
  }
  if (m_pEmbedMap) {
    return FPDFAPI_CIDFromCharCode(m_pEmbedMap, charcode);
  }
  if (!m_pMapping) {
    return (uint16_t)charcode;
  }
  if (charcode >> 16) {
    if (m_pAddMapping) {
      void* found = FXSYS_bsearch(&charcode, m_pAddMapping + 4,
                                  *(uint32_t*)m_pAddMapping, 8, CompareCID);
      if (!found) {
        if (m_pUseMap) {
          return m_pUseMap->CIDFromCharCode(charcode);
        }
        return 0;
      }
      return (uint16_t)(((uint32_t*)found)[1] % 65536 + charcode -
                        *(uint32_t*)found);
    }
    if (m_pUseMap)
      return m_pUseMap->CIDFromCharCode(charcode);
    return 0;
  }
  uint32_t CID = m_pMapping[charcode];
  if (!CID && m_pUseMap)
    return m_pUseMap->CIDFromCharCode(charcode);
  return (uint16_t)CID;
}

uint32_t CPDF_CMap::GetNextChar(const FX_CHAR* pString,
                                int nStrLen,
                                int& offset) const {
  switch (m_CodingScheme) {
    case OneByte:
      return ((uint8_t*)pString)[offset++];
    case TwoBytes:
      offset += 2;
      return ((uint8_t*)pString)[offset - 2] * 256 +
             ((uint8_t*)pString)[offset - 1];
    case MixedTwoBytes: {
      uint8_t byte1 = ((uint8_t*)pString)[offset++];
      if (!m_pLeadingBytes[byte1]) {
        return byte1;
      }
      uint8_t byte2 = ((uint8_t*)pString)[offset++];
      return byte1 * 256 + byte2;
    }
    case MixedFourBytes: {
      uint8_t codes[4];
      int char_size = 1;
      codes[0] = ((uint8_t*)pString)[offset++];
      CMap_CodeRange* pRanges = (CMap_CodeRange*)m_pLeadingBytes;
      while (1) {
        int ret = CheckCodeRange(codes, char_size, pRanges, m_nCodeRanges);
        if (ret == 0) {
          return 0;
        }
        if (ret == 2) {
          uint32_t charcode = 0;
          for (int i = 0; i < char_size; i++) {
            charcode = (charcode << 8) + codes[i];
          }
          return charcode;
        }
        if (char_size == 4 || offset == nStrLen) {
          return 0;
        }
        codes[char_size++] = ((uint8_t*)pString)[offset++];
      }
      break;
    }
  }
  return 0;
}
int CPDF_CMap::GetCharSize(uint32_t charcode) const {
  switch (m_CodingScheme) {
    case OneByte:
      return 1;
    case TwoBytes:
      return 2;
    case MixedTwoBytes:
    case MixedFourBytes:
      if (charcode < 0x100) {
        return 1;
      }
      if (charcode < 0x10000) {
        return 2;
      }
      if (charcode < 0x1000000) {
        return 3;
      }
      return 4;
  }
  return 1;
}
int CPDF_CMap::CountChar(const FX_CHAR* pString, int size) const {
  switch (m_CodingScheme) {
    case OneByte:
      return size;
    case TwoBytes:
      return (size + 1) / 2;
    case MixedTwoBytes: {
      int count = 0;
      for (int i = 0; i < size; i++) {
        count++;
        if (m_pLeadingBytes[((uint8_t*)pString)[i]]) {
          i++;
        }
      }
      return count;
    }
    case MixedFourBytes: {
      int count = 0, offset = 0;
      while (offset < size) {
        GetNextChar(pString, size, offset);
        count++;
      }
      return count;
    }
  }
  return size;
}

int CPDF_CMap::AppendChar(FX_CHAR* str, uint32_t charcode) const {
  switch (m_CodingScheme) {
    case OneByte:
      str[0] = (uint8_t)charcode;
      return 1;
    case TwoBytes:
      str[0] = (uint8_t)(charcode / 256);
      str[1] = (uint8_t)(charcode % 256);
      return 2;
    case MixedTwoBytes:
    case MixedFourBytes:
      if (charcode < 0x100) {
        CMap_CodeRange* pRanges = (CMap_CodeRange*)m_pLeadingBytes;
        int iSize = GetCharSizeImpl(charcode, pRanges, m_nCodeRanges);
        if (iSize == 0) {
          iSize = 1;
        }
        if (iSize > 1) {
          FXSYS_memset(str, 0, sizeof(uint8_t) * iSize);
        }
        str[iSize - 1] = (uint8_t)charcode;
        return iSize;
      }
      if (charcode < 0x10000) {
        str[0] = (uint8_t)(charcode >> 8);
        str[1] = (uint8_t)charcode;
        return 2;
      }
      if (charcode < 0x1000000) {
        str[0] = (uint8_t)(charcode >> 16);
        str[1] = (uint8_t)(charcode >> 8);
        str[2] = (uint8_t)charcode;
        return 3;
      }
      str[0] = (uint8_t)(charcode >> 24);
      str[1] = (uint8_t)(charcode >> 16);
      str[2] = (uint8_t)(charcode >> 8);
      str[3] = (uint8_t)charcode;
      return 4;
  }
  return 0;
}
CPDF_CID2UnicodeMap::CPDF_CID2UnicodeMap() {
  m_EmbeddedCount = 0;
}
CPDF_CID2UnicodeMap::~CPDF_CID2UnicodeMap() {}
FX_BOOL CPDF_CID2UnicodeMap::Initialize() {
  return TRUE;
}
FX_BOOL CPDF_CID2UnicodeMap::IsLoaded() {
  return m_EmbeddedCount != 0;
}
FX_WCHAR CPDF_CID2UnicodeMap::UnicodeFromCID(uint16_t CID) {
  if (m_Charset == CIDSET_UNICODE) {
    return CID;
  }
  if (CID < m_EmbeddedCount) {
    return m_pEmbeddedMap[CID];
  }
  return 0;
}

void CPDF_CID2UnicodeMap::Load(CPDF_CMapManager* pMgr,
                               CIDSet charset,
                               FX_BOOL bPromptCJK) {
  m_Charset = charset;
  FPDFAPI_LoadCID2UnicodeMap(charset, m_pEmbeddedMap, m_EmbeddedCount);
}

CIDSet CharsetFromOrdering(const CFX_ByteString& ordering) {
  for (size_t charset = 1; charset < FX_ArraySize(g_CharsetNames); ++charset) {
    if (ordering == CFX_ByteStringC(g_CharsetNames[charset]))
      return CIDSetFromSizeT(charset);
  }
  return CIDSET_UNKNOWN;
}

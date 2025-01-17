// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/fpdf_cmaps/cmap_int.h"

#include "core/fpdfapi/fpdf_font/font_int.h"
#include "core/fpdfapi/include/cpdf_modulemgr.h"

void FPDFAPI_FindEmbeddedCMap(const char* name,
                              int charset,
                              int coding,
                              const FXCMAP_CMap*& pMap) {
  pMap = NULL;
  CPDF_FontGlobals* pFontGlobals =
      CPDF_ModuleMgr::Get()->GetPageModule()->GetFontGlobals();
  const FXCMAP_CMap* pCMaps =
      pFontGlobals->m_EmbeddedCharsets[charset].m_pMapList;
  for (uint32_t i = 0; i < pFontGlobals->m_EmbeddedCharsets[charset].m_Count;
       i++) {
    if (FXSYS_strcmp(name, pCMaps[i].m_Name))
      continue;
    pMap = &pCMaps[i];
    break;
  }
}
extern "C" {
static int compareWord(const void* p1, const void* p2) {
  return (*(uint16_t*)p1) - (*(uint16_t*)p2);
}
};
extern "C" {
static int compareWordRange(const void* key, const void* element) {
  if (*(uint16_t*)key < *(uint16_t*)element) {
    return -1;
  }
  if (*(uint16_t*)key > ((uint16_t*)element)[1]) {
    return 1;
  }
  return 0;
}
};
extern "C" {
static int compareDWordRange(const void* p1, const void* p2) {
  uint32_t key = *(uint32_t*)p1;
  uint16_t hiword = (uint16_t)(key >> 16);
  uint16_t* element = (uint16_t*)p2;
  if (hiword < element[0]) {
    return -1;
  }
  if (hiword > element[0]) {
    return 1;
  }
  uint16_t loword = (uint16_t)key;
  if (loword < element[1]) {
    return -1;
  }
  if (loword > element[2]) {
    return 1;
  }
  return 0;
}
};
extern "C" {
static int compareDWordSingle(const void* p1, const void* p2) {
  uint32_t key = *(uint32_t*)p1;
  uint32_t value = ((*(uint16_t*)p2) << 16) | ((uint16_t*)p2)[1];
  if (key < value) {
    return -1;
  }
  if (key > value) {
    return 1;
  }
  return 0;
}
};
uint16_t FPDFAPI_CIDFromCharCode(const FXCMAP_CMap* pMap, uint32_t charcode) {
  if (charcode >> 16) {
    while (1) {
      if (pMap->m_DWordMapType == FXCMAP_CMap::Range) {
        uint16_t* found =
            (uint16_t*)FXSYS_bsearch(&charcode, pMap->m_pDWordMap,
                                     pMap->m_DWordCount, 8, compareDWordRange);
        if (found) {
          return found[3] + (uint16_t)charcode - found[1];
        }
      } else if (pMap->m_DWordMapType == FXCMAP_CMap::Single) {
        uint16_t* found =
            (uint16_t*)FXSYS_bsearch(&charcode, pMap->m_pDWordMap,
                                     pMap->m_DWordCount, 6, compareDWordSingle);
        if (found) {
          return found[2];
        }
      }
      if (pMap->m_UseOffset == 0) {
        return 0;
      }
      pMap = pMap + pMap->m_UseOffset;
    }
    return 0;
  }
  uint16_t code = (uint16_t)charcode;
  while (1) {
    if (!pMap->m_pWordMap) {
      return 0;
    }
    if (pMap->m_WordMapType == FXCMAP_CMap::Single) {
      uint16_t* found = (uint16_t*)FXSYS_bsearch(
          &code, pMap->m_pWordMap, pMap->m_WordCount, 4, compareWord);
      if (found) {
        return found[1];
      }
    } else if (pMap->m_WordMapType == FXCMAP_CMap::Range) {
      uint16_t* found = (uint16_t*)FXSYS_bsearch(
          &code, pMap->m_pWordMap, pMap->m_WordCount, 6, compareWordRange);
      if (found) {
        return found[2] + code - found[0];
      }
    }
    if (pMap->m_UseOffset == 0) {
      return 0;
    }
    pMap = pMap + pMap->m_UseOffset;
  }
  return 0;
}
uint32_t FPDFAPI_CharCodeFromCID(const FXCMAP_CMap* pMap, uint16_t cid) {
  while (1) {
    if (pMap->m_WordMapType == FXCMAP_CMap::Single) {
      const uint16_t* pCur = pMap->m_pWordMap;
      const uint16_t* pEnd = pMap->m_pWordMap + pMap->m_WordCount * 2;
      while (pCur < pEnd) {
        if (pCur[1] == cid) {
          return pCur[0];
        }
        pCur += 2;
      }
    } else if (pMap->m_WordMapType == FXCMAP_CMap::Range) {
      const uint16_t* pCur = pMap->m_pWordMap;
      const uint16_t* pEnd = pMap->m_pWordMap + pMap->m_WordCount * 3;
      while (pCur < pEnd) {
        if (cid >= pCur[2] && cid <= pCur[2] + pCur[1] - pCur[0]) {
          return pCur[0] + cid - pCur[2];
        }
        pCur += 3;
      }
    }
    if (pMap->m_UseOffset == 0) {
      return 0;
    }
    pMap = pMap + pMap->m_UseOffset;
  }
  while (1) {
    if (pMap->m_DWordMapType == FXCMAP_CMap::Range) {
      const uint16_t* pCur = pMap->m_pDWordMap;
      const uint16_t* pEnd = pMap->m_pDWordMap + pMap->m_DWordCount * 4;
      while (pCur < pEnd) {
        if (cid >= pCur[3] && cid <= pCur[3] + pCur[2] - pCur[1]) {
          return (((uint32_t)pCur[0] << 16) | pCur[1]) + cid - pCur[3];
        }
        pCur += 4;
      }
    } else if (pMap->m_DWordMapType == FXCMAP_CMap::Single) {
      const uint16_t* pCur = pMap->m_pDWordMap;
      const uint16_t* pEnd = pMap->m_pDWordMap + pMap->m_DWordCount * 3;
      while (pCur < pEnd) {
        if (pCur[2] == cid) {
          return ((uint32_t)pCur[0] << 16) | pCur[1];
        }
        pCur += 3;
      }
    }
    if (pMap->m_UseOffset == 0) {
      return 0;
    }
    pMap = pMap + pMap->m_UseOffset;
  }
  return 0;
}

void FPDFAPI_LoadCID2UnicodeMap(CIDSet charset,
                                const uint16_t*& pMap,
                                uint32_t& count) {
  CPDF_FontGlobals* pFontGlobals =
      CPDF_ModuleMgr::Get()->GetPageModule()->GetFontGlobals();
  pMap = pFontGlobals->m_EmbeddedToUnicodes[charset].m_pMap;
  count = pFontGlobals->m_EmbeddedToUnicodes[charset].m_Count;
}

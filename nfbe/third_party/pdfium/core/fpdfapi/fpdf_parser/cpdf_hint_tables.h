// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FPDF_PARSER_CPDF_HINT_TABLES_H_
#define CORE_FPDFAPI_FPDF_PARSER_CPDF_HINT_TABLES_H_

#include <vector>

#include "core/fpdfapi/fpdf_parser/include/ipdf_data_avail.h"
#include "core/fxcrt/include/fx_basic.h"
#include "core/fxcrt/include/fx_stream.h"

class CFX_BitStream;
class CPDF_DataAvail;
class CPDF_Dictionary;
class CPDF_Stream;

class CPDF_HintTables {
 public:
  CPDF_HintTables(CPDF_DataAvail* pDataAvail, CPDF_Dictionary* pLinearized)
      : m_pLinearizedDict(pLinearized),
        m_pDataAvail(pDataAvail),
        m_nFirstPageSharedObjs(0),
        m_szFirstPageObjOffset(0) {}
  ~CPDF_HintTables();

  FX_BOOL GetPagePos(int index,
                     FX_FILESIZE& szPageStartPos,
                     FX_FILESIZE& szPageLength,
                     uint32_t& dwObjNum);

  IPDF_DataAvail::DocAvailStatus CheckPage(
      int index,
      IPDF_DataAvail::DownloadHints* pHints);

  FX_BOOL LoadHintStream(CPDF_Stream* pHintStream);

 protected:
  FX_BOOL ReadPageHintTable(CFX_BitStream* hStream);
  FX_BOOL ReadSharedObjHintTable(CFX_BitStream* hStream, uint32_t offset);
  uint32_t GetItemLength(int index, const std::vector<FX_FILESIZE>& szArray);

 private:
  int ReadPrimaryHintStreamOffset() const;
  int ReadPrimaryHintStreamLength() const;

  CPDF_Dictionary* m_pLinearizedDict;
  CPDF_DataAvail* m_pDataAvail;
  uint32_t m_nFirstPageSharedObjs;
  FX_FILESIZE m_szFirstPageObjOffset;
  CFX_ArrayTemplate<uint32_t> m_dwDeltaNObjsArray;
  CFX_ArrayTemplate<uint32_t> m_dwNSharedObjsArray;
  CFX_ArrayTemplate<uint32_t> m_dwSharedObjNumArray;
  CFX_ArrayTemplate<uint32_t> m_dwIdentifierArray;
  std::vector<FX_FILESIZE> m_szPageOffsetArray;
  std::vector<FX_FILESIZE> m_szSharedObjOffsetArray;
};

#endif  // CORE_FPDFAPI_FPDF_PARSER_CPDF_HINT_TABLES_H_
